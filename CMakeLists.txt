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

cmake_minimum_required(VERSION 3.11.0)

# Configure c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the Project name
project(Habitify)

# Include necessary configuration and dependencies
include(${PROJECT_SOURCE_DIR}/cmake/config.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/imgui_glfw.cmake)

# Build the utilities: event_bus, ...
add_library(event_bus 
    "${PROJECT_SOURCE_DIR}/src/core/event_bus/event_bus.cpp"
)

target_include_directories( event_bus PUBLIC
    "${PROJECT_SOURCE_DIR}/"
)

# Build the core library
add_library(habitify_core 
    "${PROJECT_SOURCE_DIR}/src/core/application.cpp"
)

target_include_directories(habitify_core PUBLIC
    "${PROJECT_SOURCE_DIR}/"
)

target_link_libraries(habitify_core PUBLIC
    event_bus
)

# Build the frontend which depends on imgui, glfw and OpenGL
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

add_library(habitify_frontend
    "${PROJECT_SOURCE_DIR}/src/frontend/imgui_frontend.cpp"
    "${PROJECT_SOURCE_DIR}/src/frontend/layer_stack.cpp"
    "${PROJECT_SOURCE_DIR}/src/frontend/ping/ping_gui.cpp"
)

target_include_directories(habitify_frontend PUBLIC
    "${PROJECT_SOURCE_DIR}/"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/"
    "${PROJECT_SOURCE_DIR}/third_party/glfw/include/"
)

target_link_libraries(habitify_frontend PUBLIC 
    glfw ${GLFW_LIBRARIES} OpenGL::GL
    imgui
    event_bus
)


# Build the Application
add_executable(habitify 
    "${PROJECT_SOURCE_DIR}/src/core/entry_point.cpp"
)

target_link_libraries( habitify PUBLIC
    event_bus
    habitify_core
    habitify_frontend
)

# Optionally build the tests...