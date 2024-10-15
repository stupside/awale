# src/client/BUILD

cc_binary(
    name = "client",
    srcs = glob(["**/*.c", "**/*.h"]),
    deps = [
        "//lib/cmds:cmds_lib",
        "//lib/awale:awale_lib",
        "//lib/socket:socket_lib",
        "//lib/client:client_lib",
    ],
    linkopts = ["-pthread"],
)