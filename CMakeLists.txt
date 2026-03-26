cmake_minimum_required(VERSION 3.16)
project(trashmania LANGUAGES C CXX)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build Options
option(PRODUCTION_BUILD "Make this a production build" OFF)

# SDL Static Settings (Apply to the subdirectories)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
# If I don't have this it doesn't want to build on Linux! Something to do with the audio drivers for SDL2 being outdated
set(SDL_PIPEWIRE OFF CACHE BOOL "" FORCE) 

# Add Third Party Libraries
add_subdirectory(thirdparty/SDL-release-2.30.5)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/imgui-docking)

# Sources
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(trashmania ${MY_SOURCES})

# Include Paths
target_include_directories(trashmania PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Production vs Debug Logic
if (PRODUCTION_BUILD)
    target_compile_definitions(trashmania PRIVATE 
        PRODUCTION_BUILD=1 
        # In production looks int the folder next to the .exe
        RESOURCES_PATH="resources/"
    )
else()
    target_compile_definitions(trashmania PRIVATE 
        PRODUCTION_BUILD=0 
        # Used escaped quotes so C++ sees it as a string: "C:/path/to/resources/"
        RESOURCES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/resources/\"
        DEBUG 
    )
endif()

# Common Definitions
target_compile_definitions(trashmania PRIVATE SDL_MAIN_HANDLED)

# Platform Specific Settings (VS and Linux)
if (MSVC)
    target_compile_options(trashmania PRIVATE /arch:AVX2)
    target_compile_definitions(trashmania PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_target_properties(trashmania PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    # Windows-specific system libs for static SDL
    target_link_libraries(trashmania PRIVATE winmm imm32 version setupapi)
else()
    # Linux-specific: Static SDL often needs these
    target_link_libraries(trashmania PRIVATE pthread dl)
endif()

# Link Everything
target_link_libraries(trashmania PRIVATE
    glm
    imgui
    SDL2-static
)