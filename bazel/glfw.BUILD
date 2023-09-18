WIN32_DEFINES = [
    "_GLFW_WIN32",
]

WIN32_HDRS = [
    "src/win32_joystick.h",
    "src/win32_platform.h",
]

WIN32_SRCS = [
    "src/win32_init.c",
    "src/win32_joystick.c",
    "src/win32_monitor.c",
    "src/win32_module.c",
    "src/win32_thread.c",
    "src/win32_time.c",
    "src/win32_window.c",
    "src/wgl_context.c",
]

WIN32_LINKOPTS = [
    "-DEFAULTLIB:opengl32.lib",
    "-DEFAULTLIB:user32.lib",
    "-DEFAULTLIB:gdi32.lib",
    "-DEFAULTLIB:shell32.lib",
]

LINUX_DEFINES = [
    "_GLFW_HAS_XF86VM",
    "_GLFW_X11",
]

LINUX_HDRS = [
    "src/linux_joystick.h",
    "src/posix_thread.h",
    "src/posix_time.h",
    "src/x11_platform.h",
]

LINUX_SRCS = [
    "src/glx_context.c",
    "src/linux_joystick.c",
    "src/posix_thread.c",
    "src/posix_time.c",
    "src/x11_init.c",
    "src/x11_monitor.c",
    "src/x11_window.c",
]

# Additional include paths for X11
LINUX_INCLUDES = [
    "@X11//:include",
    "@X11_Xcursor//:include",
    "@X11_Xinerama//:include",
    "@X11_Xkb//:include",
    "@X11_Xrandr//:include",
    "@X11_Xi//:include",
    "@X11_Xshape//:include",
]

# Additional link options for X11
LINUX_LINKOPTS = [
    "-lX11",
    "-lXcursor",
    "-lXinerama",
    "-lXi",
    "-lXrandr",
    "-lXxf86vm",
    "-lXkb",
    "-lXrender",
    "-lXext",
]

cc_library(
    name = "glfw_src",
    srcs = [
        "src/context.c",
        "src/egl_context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/null_init.c",
        "src/null_joystick.c",
        "src/null_monitor.c",
        "src/null_window.c",
        "src/osmesa_context.c",
        "src/platform.c",
        "src/vulkan.c",
        "src/window.c",
        "src/xkb_unicode.c",
    ] + select({
        "@bazel_tools//src/conditions:windows": WIN32_SRCS,
        "@bazel_tools//src/conditions:linux_x86_64": LINUX_SRCS,
    }),
    hdrs = [
        "include/GLFW/glfw3.h",
        "include/GLFW/glfw3native.h",
        "src/internal.h",
        "src/mappings.h",
        "src/null_joystick.h",
        "src/null_platform.h",
        "src/platform.h",
        "src/win32_thread.h",
        "src/win32_time.h",
        "src/xkb_unicode.h",
    ] + select({
        "@bazel_tools//src/conditions:windows": WIN32_HDRS,
        "@bazel_tools//src/conditions:linux_x86_64": LINUX_HDRS,
    }),
    defines = select({
        "@bazel_tools//src/conditions:windows": WIN32_DEFINES,
        "@bazel_tools//src/conditions:linux_x86_64": LINUX_DEFINES,
    }),
    includes = select({
        "@bazel_tools//src/conditions:windows": [],
        "@bazel_tools//src/conditions:linux_x86_64": LINUX_INCLUDES,
    }),
)

cc_library(
    name = "glfw",
    hdrs = [
        "include/GLFW/glfw3.h",
        "include/GLFW/glfw3native.h",
    ],
    linkopts = select({
        "@bazel_tools//src/conditions:windows": WIN32_LINKOPTS,
        "@bazel_tools//src/conditions:linux_x86_64": LINUX_LINKOPTS,
    }),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [":glfw_src"],
)
