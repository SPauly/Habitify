load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "publisher_test",
    size = "small",
    srcs = [
        "listener_test.cpp",
        "publisher_test.cpp",
        "channel_test.cpp",
        "event_test.cpp",
        "event_bus_system_test.cpp",
    ],
    deps = [
        "//src/core/event_bus:eventbus",
        "@com_google_googletest//:gtest",
    ],
)
