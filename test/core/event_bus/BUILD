load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "event_bus_system_test",
    size = "small",
    srcs = [
        "event_bus_system_test.cpp",
    ],
    deps = [
        "//src/core/event_bus:eventbus",
        "@com_google_googletest//:gtest",
    ],
)
