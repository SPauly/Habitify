load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ping_frontend",
    srcs = [
        "ping_gui.cpp",
    ],
    hdrs = [
        "ping_gui.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/frontend:frontend_utils",
        "@imgui",
    ],
)
