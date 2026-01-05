cmake_minimum_required(VERSION 3.28)
project(atmosil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
        OVERRIDE_FIND_PACKAGE)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.91.9
)
FetchContent_MakeAvailable(imgui)
set(IMGUI_DIR ${imgui_SOURCE_DIR})

FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG v3.0
)
FetchContent_MakeAvailable(imgui-sfml)

add_executable(atmosil
        src/utils/random.cpp
        src/main.cpp
        src/atmo/Space.cpp
        src/atmo/GasRenderer.cpp
        src/atmo/AtmoDebugControl.cpp
        src/atmo/AtmoDebugControl.h
        src/atmo/ParticleSystem.cpp
        src/atmo/ParticleSystem.h)


target_compile_features(atmosil PRIVATE cxx_std_20)
target_link_libraries(atmosil PRIVATE ImGui-SFML::ImGui-SFML gmp)

set(FONT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/cabin")
set(FONT_DEST_DIR "${CMAKE_BINARY_DIR}/bin/fonts")
file(GLOB FONT_FILES "${FONT_SOURCE_DIR}/*.ttf")

add_custom_command(
        TARGET atmosil
        POST_BUILD
        # 1. Create directory explicitly at build time
        COMMAND ${CMAKE_COMMAND} -E make_directory ${FONT_DEST_DIR}
        # 2. Copy files
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_FILES} ${FONT_DEST_DIR}
        COMMENT "Copying fonts to ${FONT_DEST_DIR}"
)

# --- Shaders ---
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_DEST_DIR "${CMAKE_BINARY_DIR}/bin/shaders")
file(GLOB SHADER_FILES "${SHADER_SOURCE_DIR}/*.*")

add_custom_command(
        TARGET atmosil
        POST_BUILD
        # 1. Create directory explicitly at build time
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DEST_DIR}
        # 2. Copy files
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILES} ${SHADER_DEST_DIR}
        COMMENT "Copying shaders to ${SHADER_DEST_DIR}"
)
