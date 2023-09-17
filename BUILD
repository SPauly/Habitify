load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "Habitify",
    srcs = [
        "//src/core:habitify_main_exported",
    ],
    deps = ["//src/core:habitify_core"]
)

cc_binary(
    name = "imgui_example",
    srcs = ["third_party/imgui/examples/example_glfw_opengl3/main.cpp"],
    deps = ["@imgui//:imgui"]
)