# Habitify - Habit tracking and creating platform
# Copyright (C) 2023  Simon Pauly
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Contact via <https://github.com/SPauly/Habitify>

# Exports the imgui and glfw targets

cmake_minimum_required(VERSION 3.11.0)

# Build glfw first since this is a dependency of imgui
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/glfw)

# Build the imgui library
add_library(imgui 
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp"
)

target_include_directories(imgui PUBLIC
    "${PROJECT_SOURCE_DIR}/third_party/imgui"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends"
    "${PROJECT_SOURCE_DIR}/third_party/glfw/include"
)

# Make sure glfw is build first
add_dependencies(imgui glfw)