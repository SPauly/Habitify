load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "publisher_test",
    size = "small",
    srcs = ["publisher_test.cpp"],
    deps = [
        "//src/core/event_bus:eventbus",
        "@com_google_googletest//:gtest",
    ],
)
