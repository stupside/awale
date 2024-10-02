# lib/awale/BUILD

cc_library(
    name = "server_lib",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//lib/awale:awale_lib",
        "//lib/socket:socket_lib",
    ],
)