cmake_minimum_required(VERSION 3.21)
project(neoc VERSION 0.1.0 LANGUAGES C)

# Modern C23 standard (fallback to C17 if not available)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

# Export compile_commands.json for LSP/clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(NEOC_BUILD_TESTS "Build unit tests" OFF)
option(NEOC_ENABLE_SANITIZERS "Enable address/undefined sanitizers" OFF)
option(NEOC_ENABLE_LTO "Enable link-time optimization" ON)
option(NEOC_USE_SDL "Use SDL2 for platform abstraction" ON)

# Source files
set(SOURCES
    src/main.c
    src/bus.c
    src/neoc.c
    src/m68k.c
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Compiler warnings and features
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4 /WX
        /wd4100  # unreferenced formal parameter
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        -Wconversion -Wshadow -Wdouble-promotion
        -Wformat=2 -Wformat-truncation
        -fno-common
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

# Sanitizers (optional)
if(NEOC_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
endif()

# SDL2 for cross-platform windowing/audio (modern approach)
if(NEOC_USE_SDL)
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        target_compile_definitions(${PROJECT_NAME} PRIVATE NEOC_USE_SDL)
        target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
    else()
        message(WARNING "SDL2 not found, falling back to native platform code")
        set(NEOC_USE_SDL OFF)
    endif()
endif()

# Platform detection and linking (fallback if SDL not used)
if(NOT NEOC_USE_SDL)
    if(APPLE)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_MACOS)
        find_library(COCOA_LIBRARY Cocoa REQUIRED)
        find_library(OPENGL_LIBRARY OpenGL REQUIRED)
        find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            ${COCOA_LIBRARY}
            ${OPENGL_LIBRARY}
            ${AUDIOTOOLBOX_LIBRARY}
        )
    elseif(WIN32)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
        target_link_libraries(${PROJECT_NAME}
            opengl32
            gdi32
            winmm
            ws2_32
        )
    elseif(UNIX)
        target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
        find_package(OpenGL REQUIRED)
        find_package(X11 REQUIRED)
        find_package(Threads REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            OpenGL::GL
            ${X11_LIBRARIES}
            asound
            Threads::Threads
            m
        )
    endif()
endif()

# Release optimizations with LTO
if(NEOC_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if(lto_supported)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${lto_error}")
    endif()
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/include/version.h.in
    ${CMAKE_BINARY_DIR}/include/version.h
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)

# Tests
if(NEOC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
