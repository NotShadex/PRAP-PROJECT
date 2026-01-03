cmake_minimum_required(VERSION 3.16)
project(mygame LANGUAGES C CXX)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Build Options
option(PRODUCTION_BUILD "Make this a production build" OFF)

# 2. SDL Static Settings (Apply to the subdirectories)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

# 3. Add Third Party Libraries
add_subdirectory(thirdparty/SDL-release-2.30.5)
add_subdirectory(thirdparty/stb_image)
add_subdirectory(thirdparty/stb_truetype)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/imgui-docking)

# 4. Sources
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(mygame ${MY_SOURCES})

# 5. Include Paths
target_include_directories(mygame PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 6. Production vs Debug Logic
if (PRODUCTION_BUILD)
    target_compile_definitions(mygame PRIVATE 
        PRODUCTION_BUILD=1 
        RESOURCES_PATH="./resources/"
    )
else()
    target_compile_definitions(mygame PRIVATE 
        PRODUCTION_BUILD=0 
        RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
        DEBUG # This is what you use in your #ifdef
    )
endif()

# Common Definitions
target_compile_definitions(mygame PRIVATE SDL_MAIN_HANDLED)

# 7. Platform Specific Settings (Windows vs Linux)
if (MSVC)
    target_compile_options(mygame PRIVATE /arch:AVX2)
    target_compile_definitions(mygame PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_target_properties(mygame PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    # Windows-specific system libs for static SDL
    target_link_libraries(mygame PRIVATE winmm imm32 version setupapi)
else()
    # Linux-specific: Static SDL often needs these
    target_link_libraries(mygame PRIVATE pthread dl)
endif()

# 8. Link Everything
target_link_libraries(mygame PRIVATE
    glm
    stb_image
    stb_truetype
    imgui
    SDL2-static
)