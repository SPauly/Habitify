load("@rules_cc//cc:defs.bzl", "cc_library")

filegroup(
    name = "habitify_main_exported",
    srcs = ["entry_point.cpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "habitify_frontend",
    srcs = [
        "imgui_frontend.cpp",
        "layer_stack.cpp",
    ],
    hdrs = [
        "imgui_frontend.h",
        "layer_stack.h",
        "layer.h",
    ],
    visibility = ["//src/core/habitify_core:__pkg__"],
)

cc_library(
    name = "habitify_core",
    srcs = [
        "application.cpp",
    ],
    hdrs = [
        "application.h",
    ],
    deps = [
        ":habitify_frontend"
    ],
    visibility = ["//visibility:public"],
)
