cmake_minimum_required(VERSION 3.15)
project(WXW LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/Source)
set(SHADER_DIR ${SRC_DIR}/Shaders)
set(ASSETS_DIR ${SRC_DIR}/Assets)
set(DEPS_INCLUDE ${CMAKE_SOURCE_DIR}/Dependencies/Include)
set(DEPS_LIB ${CMAKE_SOURCE_DIR}/Dependencies/Libraries)
set(DEPS_BIN ${CMAKE_SOURCE_DIR}/Dependencies/Binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

add_compile_options(-Wall -Wno-deprecated -g)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fansi-escape-codes -fcolor-diagnostics)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${DEPS_INCLUDE})

if(WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME}
        ${DEPS_LIB}/glfw3.lib
        ${DEPS_LIB}/glew32.lib
        OpenGL::GL
    )

    file(GLOB DLL_FILES "${DEPS_BIN}/*.dll")
    if(DLL_FILES)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${DLL_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)
    find_package(GLEW QUIET)

    if(NOT glfw3_FOUND)
        message(FATAL_ERROR
            "\nGLFW not found.\n"
            "Install it with: brew install glfw\n"
            "Or ensure it is available to CMake.\n"
        )
    endif()

    if (NOT GLEW_FOUND)
        message(FATAL_ERROR
            "\nGLEW not found.\n"
            "Install it with: brew install glew\n"
            "Or ensure it’s available to CMake.\n"
        )
    endif()

    find_library(COCOA_LIB Cocoa REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)
    find_library(COREVID_LIB CoreVideo REQUIRED)
    find_library(COREFOUND_LIB CoreFoundation REQUIRED)

    target_link_libraries(${PROJECT_NAME}
        glfw
        GLEW::GLEW
        OpenGL::GL
        ${COCOA_LIB}
        ${IOKIT_LIB}
        ${COREVID_LIB}
        ${COREFOUND_LIB}
    )
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 3.3 QUIET)
    find_package(GLEW QUIET)

    if(NOT glfw3_FOUND)
        message(FATAL_ERROR
            "\nGLFW not found.\n"
            "Install it with: sudo apt install libglfw3-dev\n"
        )
    endif()

    if (NOT GLEW_FOUND)
        message(FATAL_ERROR
            "\nGLEW not found.\n"
            "Install it with: sudo apt install libglew-dev\n"
        )
    endif()

    target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        GLEW::GLEW
    )
endif()

add_custom_target(sync_runtime_files ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Source/Shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Source/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Source/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Source/Assets"
    VERBATIM
)

add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME} sync_runtime_files
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set(CMAKE_VERBOSE_MAKEFILE ON)