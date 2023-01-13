/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PageNode.h"
#include "SectionNode.h"

namespace Manual {

ErrorOr<String> PageNode::path() const
{
    return TRY(String::formatted("{}/{}.md", TRY(parent()->path()), TRY(name())));
}

ErrorOr<NonnullRefPtr<PageNode>> PageNode::help_index_page()
{
    static NonnullRefPtr<PageNode> const help_index_page = TRY(try_make_ref_counted<PageNode>(sections[7 - 1], TRY(String::from_utf8("Help-index"sv))));
    return help_index_page;
}

}
