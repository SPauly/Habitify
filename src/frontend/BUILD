load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "frontend_utils",
    hdrs = [
        "layer.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "imgui_frontend",
    srcs = [
        "imgui_frontend.cpp",
        "layer_stack.cpp",
    ],
    hdrs = [
        "imgui_frontend.h",
        "layer_stack.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":frontend_utils",
        "//src/debug:debug_tools",
        "//src/ping:ping_frontend",
        "@glfw",
        "@imgui",
    ],
)
