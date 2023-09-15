workspace(name = "habitify")

new_local_repository(
    name = "glfw",
    path = "third_party/glfw",
    build_file = "bazel/glfw.BUILD",
)

new_local_repository(
    name = "imgui",
    path = "third_party/imgui",
    build_file = "bazel/imgui.BUILD",
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