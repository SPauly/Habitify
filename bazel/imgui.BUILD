load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "imgui",
    srcs = [
        "imgui.cpp",
        "imgui_demo.cpp",
        "imgui_draw.cpp",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        "backends/imgui_impl_opengl3.cpp",
        "backends/imgui_impl_glfw.cpp",
    ],
    hdrs = glob([
        "*.h",
        "backends/*.h",
    ]),
    includes = [
        ".",
        "backends",
        "@glfw//include",
    ],
    linkopts = ["-DEFAULTLIB:opengl32.lib"],
    visibility = ["//visibility:public"],
    deps = ["@glfw//:glfw"
    ],
)