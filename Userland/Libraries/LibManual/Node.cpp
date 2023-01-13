/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * Copyright (c) 2022, kleines Filmröllchen <filmroellchen@serenityos.org>
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Node.h"
#include "PageNode.h"
#include "SectionNode.h"
#include "SubsectionNode.h"
#include <AK/Assertions.h>
#include <AK/LexicalPath.h>
#include <AK/Optional.h>
#include <AK/QuickSort.h>
#include <AK/StringView.h>
#include <AK/URL.h>
#include <LibCore/DirIterator.h>
#include <LibCore/File.h>
#include <LibCore/Stream.h>
#include <LibManual/Path.h>

namespace Manual {

ErrorOr<Span<NonnullRefPtr<Node>>> Node::children() const
{
    if (is_page())
        return Span<NonnullRefPtr<Node>> {};

    if (!m_reified) {
        m_reified = true;

        auto own_path = TRY(path());
        Core::DirIterator dir_iter { own_path.to_deprecated_string(), Core::DirIterator::Flags::SkipDots };

        struct Child {
            NonnullRefPtr<Node> node;
            String name_for_sorting;
        };
        Vector<Child> children;

        while (dir_iter.has_next()) {
            LexicalPath lexical_path(dir_iter.next_path());
            if (lexical_path.extension() != "md") {
                if (Core::File::is_directory(LexicalPath::absolute_path(own_path.to_deprecated_string(), lexical_path.string()))) {
                    dbgln("Found subsection {}", lexical_path);
                    children.append({ .node = TRY(try_make_ref_counted<SubsectionNode>(*this, lexical_path.title())),
                        .name_for_sorting = TRY(String::from_utf8(lexical_path.title())) });
                }
            } else {
                children.append({ .node = TRY(try_make_ref_counted<PageNode>(*this, TRY(String::from_utf8(lexical_path.title())))),
                    .name_for_sorting = TRY(String::from_utf8(lexical_path.title())) });
            }
        }

        quick_sort(children, [](auto const& a, auto const& b) { return a.name_for_sorting < b.name_for_sorting; });

        m_children.ensure_capacity(children.size());
        for (auto child : children)
            m_children.unchecked_append(move(child.node));
    }

    return m_children.span();
}

ErrorOr<NonnullRefPtr<PageNode>> Node::try_create_from_query(Vector<StringView, 2> const& query_parameters)
{
    if (query_parameters.size() > 2)
        return Error::from_string_literal("Queries longer than 2 strings are not supported yet");

    auto query_parameter_iterator = query_parameters.begin();

    if (query_parameter_iterator.is_end())
        return PageNode::help_index_page();

    auto first_query_parameter = *query_parameter_iterator;
    ++query_parameter_iterator;
    if (query_parameter_iterator.is_end()) {
        // [/path/to/docs.md]
        auto path_from_query = LexicalPath { first_query_parameter };
        if (path_from_query.is_absolute()
            && path_from_query.is_child_of(manual_base_path)
            && path_from_query.extension() == "md"sv) {
            auto section_directory = path_from_query.parent();
            auto man_string_location = section_directory.basename().find("man"sv);
            if (!man_string_location.has_value())
                return Error::from_string_literal("Page is inside invalid section");
            auto section_name = section_directory.basename().substring_view(man_string_location.value() + 3);
            auto section = TRY(SectionNode::try_create_from_number(section_name));
            return try_make_ref_counted<PageNode>(section, TRY(String::from_utf8(path_from_query.title())));
        }

        // [page] (in any section)
        Optional<NonnullRefPtr<PageNode>> maybe_page;
        for (auto const& section : sections) {
            auto const page = TRY(try_make_ref_counted<PageNode>(section, TRY(String::from_utf8(first_query_parameter))));
            if (Core::File::exists(TRY(page->path()))) {
                maybe_page = page;
                break;
            }
        }
        if (maybe_page.has_value())
            return maybe_page.release_value();
        return Error::from_string_literal("Page not found");
    }
    // [section] [name]
    auto second_query_parameter = *query_parameter_iterator;
    auto section = TRY(SectionNode::try_create_from_number(first_query_parameter));
    auto const page = TRY(try_make_ref_counted<PageNode>(section, TRY(String::from_utf8(second_query_parameter))));
    if (Core::File::exists(TRY(page->path())))
        return page;
    return Error::from_string_literal("Page doesn't exist in section");
}

ErrorOr<NonnullRefPtr<Node>> Node::try_find_from_help_url(URL const& url)
{
    if (url.host() != "man")
        return Error::from_string_view("Bad help operation"sv);
    if (url.paths().size() < 2)
        return Error::from_string_view("Bad help page URL"sv);

    auto paths = url.paths();
    auto const section = paths.take_first();
    auto maybe_section_number = section.to_uint();
    if (!maybe_section_number.has_value())
        return Error::from_string_view("Bad section number"sv);
    auto section_number = maybe_section_number.value();
    if (section_number > number_of_sections)
        return Error::from_string_view("Section number out of bounds"sv);

    NonnullRefPtr<Node> current_node = sections[section_number - 1];

    while (!paths.is_empty()) {
        auto next_path_segment = TRY(String::from_deprecated_string(paths.take_first()));
        auto children = TRY(current_node->children());
        for (auto const& child : children) {
            if (TRY(child->name()) == next_path_segment) {
                current_node = child;
                break;
            }
        }
    }
    return current_node;
}

}
