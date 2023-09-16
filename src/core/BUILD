load("@rules_cc//cc:defs.bzl", "cc_library")

filegroup(
    name = "habitify_main_exported",
    srcs = ["entry_point.cpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "habitify_core",
    srcs = ["application.cpp"],
    hdrs = ["application.h"],
    visibility = ["//visibility:public"],
)
