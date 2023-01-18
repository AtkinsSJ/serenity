/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * Copyright (c) 2022, kleines Filmröllchen <filmroellchen@serenityos.org>
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ManualModel.h"
#include <AK/Try.h>
#include <AK/Utf8View.h>
#include <LibCore/DirIterator.h>
#include <LibCore/File.h>
#include <LibManual/Node.h>
#include <LibManual/PageNode.h>
#include <LibManual/Path.h>
#include <LibManual/SectionNode.h>

class ManualNode : public RefCounted<ManualNode> {
public:
    static ErrorOr<NonnullRefPtr<ManualNode>> create(RefPtr<ManualNode> parent, String name, Optional<String> page_path)
    {
        return adopt_nonnull_ref_or_enomem(new (nothrow) ManualNode(move(parent), move(name), move(page_path)));
    }
    ~ManualNode() = default;

    String const& name() const { return m_name; }
    RefPtr<ManualNode> const& parent() const { return m_parent; }

    bool has_page() const { return m_page_path.has_value(); }
    Optional<String const&> page_path() const
    {
        return m_page_path.has_value() ? m_page_path.value() : Optional<String const&> {};
    }

    bool has_children() const { return !m_children.is_empty(); }
    NonnullRefPtrVector<ManualNode> const& children() const { return m_children; }
    void set_children(NonnullRefPtrVector<ManualNode> children) { m_children = move(children); }

    bool is_open() const { return m_is_open; }
    void set_open(bool open) { m_is_open = open; }

private:
    ManualNode(RefPtr<ManualNode> parent, String name, Optional<String> page_path)
        : m_parent(move(parent))
        , m_name(move(name))
        , m_page_path(move(page_path))
    {
    }

    RefPtr<ManualNode> m_parent;
    String m_name;
    Optional<String> m_page_path;
    NonnullRefPtrVector<ManualNode> m_children;
    bool m_is_open { false };
};

ErrorOr<NonnullRefPtr<ManualModel>> ManualModel::create()
{
    auto section_open_icon = TRY(Gfx::Bitmap::try_load_from_file("/res/icons/16x16/book-open.png"sv));
    auto section_icon = TRY(Gfx::Bitmap::try_load_from_file("/res/icons/16x16/book.png"sv));
    auto page_icon = TRY(Gfx::Bitmap::try_load_from_file("/res/icons/16x16/filetype-unknown.png"sv));

    auto manual_model = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) ManualModel(move(section_open_icon), move(section_icon), move(page_icon))));
    TRY(manual_model->load_sections());
    manual_model->dump();
    return manual_model;
}

ManualModel::ManualModel(NonnullRefPtr<Gfx::Bitmap> section_open_icon, NonnullRefPtr<Gfx::Bitmap> section_icon, NonnullRefPtr<Gfx::Bitmap> page_icon)
{
    m_section_open_icon.set_bitmap_for_size(16, move(section_open_icon));
    m_section_icon.set_bitmap_for_size(16, move(section_icon));
    m_page_icon.set_bitmap_for_size(16, move(page_icon));
}

ManualModel::~ManualModel() {

};

Optional<GUI::ModelIndex> ManualModel::index_from_path(StringView path) const
{
    // The first slice removes the man pages base path plus the `/man` from the main section subdirectory.
    // The second slice removes the trailing `.md`.
    VERIFY(path.ends_with(".md"sv));
    auto path_without_base = path.substring_view(Manual::manual_base_path.string().length() + 4);
    auto url = URL::create_with_help_scheme(path_without_base.substring_view(0, path_without_base.length() - 3), {}, "man");

    auto maybe_index = index_from_url(url);
    if (maybe_index.is_error())
        return {};

    return maybe_index.value();
}

ErrorOr<GUI::ModelIndex> ManualModel::index_from_url(URL url) const
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
    if (section_number < 1 || section_number > m_manual_sections.size())
        return Error::from_string_view("Section number out of bounds"sv);

    auto current_node = m_manual_sections[section_number - 1];

    // If we have no further paths, we're a top-level section.
    if (paths.is_empty())
        return index(static_cast<int>(section_number - 1), 0);

    int index_within_parent = 0;
    while (!paths.is_empty()) {
        auto next_path_segment = paths.take_first();
        auto children = current_node->children();
        index_within_parent = 0;
        bool found = false;
        for (auto const& child : children) {
            if (child.name() == next_path_segment.view()) {
                current_node = child;
                found = true;
                break;
            }
            index_within_parent++;
        }

        if (!found)
            return Error::from_string_view("No page matching URL"sv);
    }

    return create_index(index_within_parent, 0, current_node.ptr());
}

Optional<String const&> ManualModel::page_name(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return {};
    auto* node = static_cast<ManualNode const*>(index.internal_data());
    return node->name();
}

Optional<String const&> ManualModel::page_path(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return {};
    auto* node = static_cast<ManualNode const*>(index.internal_data());
    return node->page_path();
}

ErrorOr<StringView> ManualModel::page_view(String const& path) const
{
    if (path.is_empty())
        return StringView {};

    {
        // Check if we've got it cached already.
        auto mapped_file = m_mapped_files.get(path);
        if (mapped_file.has_value())
            return StringView { mapped_file.value()->bytes() };
    }

    auto file = TRY(Core::MappedFile::map(path));

    StringView view { file->bytes() };
    m_mapped_files.set(path, move(file));
    return view;
}

Optional<String> ManualModel::page_and_section(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return {};
    auto* node = static_cast<ManualNode const*>(index.internal_data());
    auto& parent = node->parent();
    if (parent == nullptr)
        return node->name();

    auto page_and_section = String::formatted("{}({})", node->name(), parent->name());
    if (page_and_section.is_error())
        return {};
    return page_and_section.release_value();
}

GUI::ModelIndex ManualModel::index(int row, int column, GUI::ModelIndex const& parent_index) const
{
    if (!parent_index.is_valid())
        return create_index(row, column, m_manual_sections[row].ptr());
    auto* parent = static_cast<ManualNode const*>(parent_index.internal_data());
    if (!parent->has_children())
        return {};
    auto& child = parent->children()[row];
    return create_index(row, column, &child);
}

GUI::ModelIndex ManualModel::parent_index(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return {};
    auto* child = static_cast<ManualNode const*>(index.internal_data());
    auto parent = child->parent();
    if (parent == nullptr)
        return {};

    if (parent->parent() == nullptr) {
        for (size_t row = 0; row < m_manual_sections.size(); row++)
            if (m_manual_sections[row] == parent)
                return create_index(row, 0, parent);
        VERIFY_NOT_REACHED();
    }
    auto& children = parent->parent()->children();
    for (size_t row = 0; row < children.size(); row++) {
        auto& child_at_row = children[row];
        if (child_at_row == parent)
            return create_index(row, 0, parent);
    }
    VERIFY_NOT_REACHED();
}

int ManualModel::row_count(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return static_cast<int>(m_manual_sections.size());
    auto* node = static_cast<ManualNode const*>(index.internal_data());
    return static_cast<int>(node->children().size());
}

int ManualModel::column_count(GUI::ModelIndex const&) const
{
    return 1;
}

GUI::Variant ManualModel::data(GUI::ModelIndex const& index, GUI::ModelRole role) const
{
    auto* node = static_cast<ManualNode const*>(index.internal_data());

    switch (role) {
    case GUI::ModelRole::Search:
        if (!node->has_page())
            return {};
        if (auto path = page_path(index); path.has_value())
            if (auto page = page_view(path.release_value()); !page.is_error())
                // FIXME: We already provide String, but GUI::Variant still needs DeprecatedString.
                return DeprecatedString(page.release_value());
        return {};
    case GUI::ModelRole::Display:
        return node->name();
    case GUI::ModelRole::Icon:
        if (!node->has_children())
            return m_page_icon;
        if (node->is_open())
            return m_section_open_icon;
        return m_section_icon;
    default:
        return {};
    }
}

void ManualModel::update_section_node_on_toggle(GUI::ModelIndex const& index, bool const open)
{
    auto* node = static_cast<ManualNode*>(index.internal_data());
    if (node->has_children())
        node->set_open(open);
}

TriState ManualModel::data_matches(GUI::ModelIndex const& index, const GUI::Variant& term) const
{
    auto name = page_name(index);
    if (!name.has_value())
        return TriState::False;

    if (name.value().bytes_as_string_view().contains(term.as_string(), CaseSensitivity::CaseInsensitive))
        return TriState::True;

    auto path = page_path(index);
    // NOTE: This is slightly inaccurate, as page_path can also fail due to OOM. We consider it acceptable to have a data mismatch in that case.
    if (!path.has_value())
        return TriState::False;
    auto view_result = page_view(path.release_value());
    if (view_result.is_error() || view_result.value().is_empty())
        return TriState::False;

    return view_result.value().contains(term.as_string(), CaseSensitivity::CaseInsensitive) ? TriState::True : TriState::False;
}

ErrorOr<void> ManualModel::load_sections()
{
    auto load_root_section = [&](StringView section_number, StringView section_display_name) -> ErrorOr<NonnullRefPtr<ManualNode>> {
        auto current_path = DeprecatedString::formatted("{}/man{}", Manual::manual_base_path, section_number);
        Core::DirIterator dir_iterator { current_path, Core::DirIterator::Flags::SkipParentAndBaseDir };

        struct Child {
            String name;
            Optional<String> page_path {};
            RefPtr<ManualNode> node { nullptr };
        };
        Vector<Child> children;

        while (dir_iterator.has_next()) {
            auto path = dir_iterator.next_full_path();
            LexicalPath lexical_path { path };
            if (Core::File::is_directory(path)) {
                TRY(children.try_append({ .name = TRY(String::from_utf8(lexical_path.title())) }));

                // TODO: Recurse!
            } else {
                auto section_with_same_name = children.find_if([&](Child const& child) {
                    return child.name == lexical_path.title();
                });
                if (!section_with_same_name.is_end()) {
                    section_with_same_name->page_path.emplace(TRY(String::from_deprecated_string(path)));
                    continue;
                }

                TRY(children.try_append({ TRY(String::from_utf8(lexical_path.title())), TRY(String::from_deprecated_string(path)) }));
            }
        }

        auto section_node = TRY(ManualNode::create(nullptr, TRY(String::from_utf8(section_display_name)), {}));

        NonnullRefPtrVector<ManualNode> node_children;
        TRY(node_children.try_ensure_capacity(children.size()));
        for (const auto& item : children) {
            node_children.unchecked_append(TRY(ManualNode::create(section_node, move(item.name), move(item.page_path))));
        }
        section_node->set_children(move(node_children));

        return section_node;
    };

    m_manual_sections[0] = TRY(load_root_section("1"sv, TRY(String::from_utf8("1 - User Programs"sv))));
    m_manual_sections[1] = TRY(load_root_section("2"sv, TRY(String::from_utf8("2 - System Calls"sv))));
    m_manual_sections[2] = TRY(load_root_section("3"sv, TRY(String::from_utf8("3 - Library Functions"sv))));
    m_manual_sections[3] = TRY(load_root_section("4"sv, TRY(String::from_utf8("4 - Special Files"sv))));
    m_manual_sections[4] = TRY(load_root_section("5"sv, TRY(String::from_utf8("5 - File Formats"sv))));
    m_manual_sections[5] = TRY(load_root_section("6"sv, TRY(String::from_utf8("6 - Games"sv))));
    m_manual_sections[6] = TRY(load_root_section("7"sv, TRY(String::from_utf8("7 - Miscellanea"sv))));
    m_manual_sections[7] = TRY(load_root_section("8"sv, TRY(String::from_utf8("8 - Sysadmin Tools"sv))));

    return {};
}

void ManualModel::dump() const
{
    dbgln("--- Dumping Manual Model ---");
    auto dump = [&](ManualNode const& node, i32 depth, auto recursive_dump) -> void {
        dbgln("{}Node `{}`({:p}), page: `{}`, parent: `{}`, children: {}", DeprecatedString::repeated("  "sv, depth), node.name(), &node, node.has_page() ? node.page_path()->bytes_as_string_view() : "[NONE]"sv, node.parent().is_null() ? "[NONE]"sv : node.parent()->name(), node.children().size());
        for (const auto& item : node.children())
            recursive_dump(item, depth + 1, recursive_dump);
    };
    for (auto const& section : m_manual_sections) {
        dump(*section, 0, dump);
    }
    dbgln("--- Dump Completed ---");
}
