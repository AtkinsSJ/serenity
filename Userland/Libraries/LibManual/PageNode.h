/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullRefPtr.h>
#include <LibManual/Node.h>

namespace Manual {

class SectionNode;

class PageNode : public Node {
public:
    virtual ~PageNode() override = default;

    PageNode(NonnullRefPtr<Node> section, String page)
        : Node(section.ptr(), page, MUST(String::formatted("{}/{}.md", MUST(section->path()), page)))
    {
    }

    virtual bool is_page() const override { return true; }
    virtual PageNode const* document() const override { return this; };

    static ErrorOr<NonnullRefPtr<PageNode>> help_index_page();
};

}
