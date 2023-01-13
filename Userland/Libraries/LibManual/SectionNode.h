/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/String.h>
#include <LibManual/Node.h>
#include <LibManual/Path.h>

namespace Manual {

class SectionNode : public Node {
public:
    virtual ~SectionNode() override = default;

    SectionNode(RefPtr<Node> parent, StringView section, StringView name)
        : Node(move(parent), MUST(String::formatted("{}. {}", section, name)), MUST(String::formatted("{}/{}{}", manual_base_path, top_level_section_prefix, section)))
        , m_section(MUST(String::from_utf8(section)))
    {
    }

    SectionNode(RefPtr<Node> parent, String name, String path)
        : Node(move(parent), name, move(path))
        , m_section(name)
    {
    }

    String const& section_name() const { return m_section; }
    virtual PageNode const* document() const override { return nullptr; }

    static ErrorOr<NonnullRefPtr<SectionNode>> try_create_from_number(StringView section_number);

protected:
    // In this class, the section is a number, but in lower sections it might be the same as the name.
    String m_section;
};

constexpr size_t number_of_sections = 8;

extern Array<NonnullRefPtr<SectionNode>, number_of_sections> const sections;

constexpr Array<StringView, number_of_sections> const section_numbers = {
    "1"sv,
    "2"sv,
    "3"sv,
    "4"sv,
    "5"sv,
    "6"sv,
    "7"sv,
    "8"sv,
};

}
