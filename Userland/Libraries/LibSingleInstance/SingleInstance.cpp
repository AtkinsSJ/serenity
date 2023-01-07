/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SingleInstance.h"
#include <AK/String.h>
#include <LibCore/LocalServer.h>
#include <LibCore/Stream.h>

namespace Core {

SingleInstanceServer::SingleInstanceServer(NonnullRefPtr<Core::LocalServer> local_server)
    : m_local_server(move(local_server))
{
}

ConnectionToSingleInstance::ConnectionToSingleInstance(NonnullOwnPtr<Core::Stream::LocalSocket> socket)
    : IPC::ConnectionToServer<SingleInstanceClientEndpoint, SingleInstanceServerEndpoint>(*this, move(socket))
{
}

ErrorOr<NonnullOwnPtr<SingleInstanceServer>> ensure_single_instance(StringView identifier, Span<StringView> const& arguments)
{
    auto socket_path = TRY(String::formatted("/tmp/single-instance/{}", identifier));

    (void)System::mkdir("/tmp/single-instance/"sv, 0700);

    auto local_server = TRY(Core::LocalServer::try_create());
    if (local_server->listen(socket_path.to_deprecated_string())) {
        // We made a server, yay!
        return adopt_nonnull_own_or_enomem(new (nothrow) SingleInstanceServer(move(local_server)));
    }
    // We couldn't create a server, so assume that it already exists.
    auto connection_to_instance = TRY(ConnectionToSingleInstance::try_create(socket_path));

    Vector<DeprecatedString> argument_strings;
    TRY(argument_strings.try_ensure_capacity(arguments.size()));
    for (auto const& argument : arguments)
        argument_strings.unchecked_append(argument);

    connection_to_instance->async_notify_new_launch(move(argument_strings));
    dbgln("We sent our arguments to the single instance. Quitting!");
    exit(0);
}

}
