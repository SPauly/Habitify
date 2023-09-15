http_archive(
    name = "com_github_grpc_grpc",
    urls = [
        "https://github.com/grpc/grpc/archive/0f8cb206ef897f9cff2206e06a1a5303dbd4aeef.tar.gz",
    ],
    strip_prefix = "grpc-0f8cb206ef897f9cff2206e06a1a5303dbd4aeef",
)
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()