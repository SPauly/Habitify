load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "debug_tools",
    srcs = [
        "debug_gui.cpp",
    ],
    hdrs = [
        "debug_gui.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/frontend:frontend_utils",
        "@imgui",
    ],
)
