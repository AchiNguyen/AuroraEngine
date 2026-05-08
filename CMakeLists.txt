if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "vcpkg toolchain file")
endif()

cmake_minimum_required(VERSION 3.21)
project(AuroraEngine
    VERSION 0.1.0
    DESCRIPTION "Aurora real-time rendering engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:preprocessor /MP)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(AuroraEngine
    src/main.cpp
    src/window.cpp
    src/shader.cpp
    src/mesh.cpp
    src/cube.cpp
    src/camera.cpp
    src/light.cpp
    src/material.cpp
    src/texture.cpp
    src/texture_cache.cpp
    src/model.cpp
    src/stb_image_impl.cpp
    src/debug_ui.cpp
)

target_include_directories(AuroraEngine
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(AuroraEngine
    PRIVATE
        glfw
        glad::glad
        glm::glm
        spdlog::spdlog
        assimp::assimp
        imgui::imgui
)

target_compile_definitions(AuroraEngine
    PRIVATE
        $<$<CONFIG:Debug>:AURORA_DEBUG>
        GLFW_INCLUDE_NONE
)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AuroraEngine)
    set_target_properties(AuroraEngine PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

add_custom_command(TARGET AuroraEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:AuroraEngine>/shaders
    COMMENT "Mirroring shaders/ next to AuroraEngine.exe"
)

add_custom_command(TARGET AuroraEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:AuroraEngine>/assets
    COMMENT "Mirroring assets/ next to AuroraEngine.exe"
)
