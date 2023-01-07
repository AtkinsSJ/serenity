/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/StringView.h>
#include <LibCore/LocalServer.h>
#include <LibIPC/ConnectionToServer.h>
#include <LibSingleInstance/SingleInstanceClientEndpoint.h>
#include <LibSingleInstance/SingleInstanceServerEndpoint.h>

namespace Core {

class SingleInstanceServer final {
public:
    SingleInstanceServer(NonnullRefPtr<Core::LocalServer>);

    Function<void(Span<StringView> const&)> on_new_instance;

private:
    NonnullRefPtr<Core::LocalServer> m_local_server;
};

class ConnectionToSingleInstance final
    : public IPC::ConnectionToServer<SingleInstanceClientEndpoint, SingleInstanceServerEndpoint>
    , public SingleInstanceClientEndpoint {
    C_OBJECT_ABSTRACT(ConnectionToSingleInstance)
public:
    // Note: Copied from IPC_CLIENT_CONNECTION macro, so we can pass in a custom socket_path.
    static ErrorOr<NonnullRefPtr<ConnectionToSingleInstance>> try_create(StringView socket_path)
    {
        auto parsed_socket_path = TRY(Core::SessionManagement::parse_path_with_sid(socket_path));
        auto socket = TRY(Core::Stream::LocalSocket::connect(move(parsed_socket_path))); /* We want to rate-limit our clients */
        TRY(socket->set_blocking(true));

        return adopt_nonnull_ref_or_enomem(new (nothrow) ConnectionToSingleInstance(move(socket)));
    }

private:
    ConnectionToSingleInstance(NonnullOwnPtr<Core::Stream::LocalSocket>);
};

/// Ensure that only one instance of this application is running, identified with `identifier`.
/// If an existing instance is found, send a message to its EventLoop with the current
/// application's arguments, and exit. If no existing instance is found, leave a socket at
/// /tmp/single-instance/{identifier} for future calls to use.
ErrorOr<NonnullOwnPtr<SingleInstanceServer>> ensure_single_instance(StringView identifier, Span<StringView> const& arguments);

}
