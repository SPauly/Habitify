workspace(name = "habitify")

new_local_repository(
    name = "glfw",
    build_file = "bazel/glfw.BUILD",
    path = "third_party/glfw",
)

new_local_repository(
    name = "imgui",
    build_file = "bazel/imgui.BUILD",
    path = "third_party/imgui",
)

# install grpc
local_repository(
    name = "com_github_grpc_grpc",
    path = "third_party/grpc",
)

# install grpc dependencies
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()
