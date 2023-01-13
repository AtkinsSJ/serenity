/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullRefPtrVector.h>
#include <AK/RefCounted.h>
#include <AK/RefPtr.h>
#include <AK/String.h>
#include <AK/StringView.h>

namespace Manual {

class PageNode;

class Node : public RefCounted<Node> {
public:
    virtual ~Node() = default;

    ErrorOr<Span<NonnullRefPtr<Node>>> children() const;
    Node const* parent() const { return m_parent; };
    ErrorOr<String> name() const { return m_name; };
    virtual bool is_page() const { return false; }
    bool is_open() const { return m_is_open; }
    void set_open(bool open) { m_is_open = open; }
    ErrorOr<String> path() const { return m_path; };
    virtual PageNode const* document() const = 0;

    // Backend for the command-line argument format that Help and man accept. Handles:
    // [/path/to/documentation.md] (no second argument)
    // [page] (no second argument) - will find first section with that page
    // [section] [page]
    // Help can also (externally) handle search queries, which is not possible (yet) in man.
    static ErrorOr<NonnullRefPtr<PageNode>> try_create_from_query(Vector<StringView, 2> const& query_parameters);

    // Finds a page via the help://man/<number>/<subsections...>/page URLs.
    // This will automatically start discovering pages by inspecting the filesystem.
    static ErrorOr<NonnullRefPtr<Node>> try_find_from_help_url(URL const&);

protected:
    explicit Node(RefPtr<Node> parent, String name, String path);

private:
    RefPtr<Node> m_parent;
    String m_name;
    String m_path;
    mutable NonnullRefPtrVector<Node> m_children;
    mutable bool m_reified { false };
    bool m_is_open { false };
};

}
