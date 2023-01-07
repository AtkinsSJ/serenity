compile_ipc(SingleInstanceClient.ipc SingleInstanceClientEndpoint.h)
compile_ipc(SingleInstanceServer.ipc SingleInstanceServerEndpoint.h)

set(SOURCES
    SingleInstance.cpp
)

set(GENERATED_SOURCES
    SingleInstanceClientEndpoint.h
    SingleInstanceServerEndpoint.h
)

serenity_lib(LibSingleInstance singleinstance)
target_link_libraries(LibSingleInstance PRIVATE LibCore LibIPC)
