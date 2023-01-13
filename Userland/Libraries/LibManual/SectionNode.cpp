/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SectionNode.h"
#include "PageNode.h"
#include "Path.h"

namespace Manual {

ErrorOr<NonnullRefPtr<SectionNode>> SectionNode::try_create_from_number(StringView section)
{
    auto maybe_section_number = section.to_uint<u32>();
    if (!maybe_section_number.has_value())
        return Error::from_string_literal("Section is not a number");
    auto section_number = maybe_section_number.release_value();
    if (section_number > number_of_sections)
        return Error::from_string_literal("Section number too large");
    return sections[section_number - 1];
}

ErrorOr<String> SectionNode::path() const
{
    return String::formatted("{}/{}{}", manual_base_path, top_level_section_prefix, m_section);
}

ErrorOr<String> SectionNode::name() const
{
    return String::formatted("{}. {}", m_section, m_name);
}

void SectionNode::set_open(bool open)
{
    if (m_open == open)
        return;
    m_open = open;
}

Array<NonnullRefPtr<SectionNode>, number_of_sections> const sections = { {
    make_ref_counted<SectionNode>(nullptr, "1"sv, "User Programs"sv),
    make_ref_counted<SectionNode>(nullptr, "2"sv, "System Calls"sv),
    make_ref_counted<SectionNode>(nullptr, "3"sv, "Library Functions"sv),
    make_ref_counted<SectionNode>(nullptr, "4"sv, "Special Files"sv),
    make_ref_counted<SectionNode>(nullptr, "5"sv, "File Formats"sv),
    make_ref_counted<SectionNode>(nullptr, "6"sv, "Games"sv),
    make_ref_counted<SectionNode>(nullptr, "7"sv, "Miscellanea"sv),
    make_ref_counted<SectionNode>(nullptr, "8"sv, "Sysadmin Tools"sv),
} };

}
