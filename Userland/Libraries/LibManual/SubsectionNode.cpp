/*
 * Copyright (c) 2022, kleines Filmröllchen <filmroellchen@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SubsectionNode.h"
#include "PageNode.h"
#include <AK/TypeCasts.h>

namespace Manual {

SubsectionNode::SubsectionNode(NonnullRefPtr<Node> parent, StringView name)
    : SectionNode(parent, MUST(String::from_utf8(name)), MUST(String::formatted("{}/{}", parent->path(), name)))
{
}

PageNode const* SubsectionNode::document() const
{
    auto maybe_siblings = parent()->children();
    if (maybe_siblings.is_error())
        return nullptr;
    auto siblings = maybe_siblings.release_value();
    for (auto const& sibling : siblings) {
        if (&*sibling == this)
            continue;
        auto sibling_name = sibling->name();
        if (sibling_name == name() && is<PageNode>(*sibling))
            return static_cast<PageNode*>(&*sibling);
    }
    return nullptr;
}

}
