# src/server/BUILD

cc_binary(
    name = "server",
    srcs = glob(["*.c", "*.h"]),
    deps = [
        "//lib/cmds:cmds_lib",
        "//lib/awale:awale_lib",
        "//lib/socket:socket_lib",
        "//lib/server:server_lib",
    ],
)