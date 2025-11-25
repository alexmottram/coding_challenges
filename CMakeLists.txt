cmake_minimum_required(VERSION 4.0)
project(coding_challenges)

set(CMAKE_CXX_STANDARD 20)

# GLFW setup
add_subdirectory(glfw)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# OpenGL setup
find_package(OpenGL REQUIRED)

set(test_tools doctest.h doctest.cpp)

set(utils
    utils/utils.h
    utils/std_extensions.h
    utils/precompile_header.cpp utils/precompile_header.h
    utils/prime_utils.cpp
    utils/prime_utils.h
        utils/guis/progress_bar.h
)

set(utils_unittests
    doctest.h doctest.cpp
    utils_tests/prime_utils_tests.cpp)

set(euler_problems
    challenges/euler/problem_1/problem_1.cpp
    challenges/euler/problem_1/problem_1.h
    challenges/euler/problem_2/problem_2.cpp
    challenges/euler/problem_2/problem_2.h
    challenges/euler/problem_3/problem_3.cpp
    challenges/euler/problem_3/problem_3.h
    challenges/euler/problem_4/problem_4.cpp
    challenges/euler/problem_4/problem_4.h
    challenges/euler/problem_5/problem_5.cpp
    challenges/euler/problem_5/problem_5.h
    challenges/euler/problem_6/problem_6.cpp
    challenges/euler/problem_6/problem_6.h
    challenges/euler/all_euler_solutions.h
)

set(euler_doctest
    challenges/euler/euler_test_cases.cpp
)

set(imgui
    imgui/imconfig.h
    imgui/imgui.h imgui/imgui.cpp
    imgui/imgui_internal.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/backends/imgui_impl_opengl3.h imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_glfw.h imgui/backends/imgui_impl_glfw.cpp
)

add_executable(
    Euler-Solutions
    "${test_tools}"
    "${utils}"
    "${euler_problems}"
    "${euler_doctest}"
)

add_executable(
    Util-Tests
    "${test_tools}"
    "${utils}"
    "${utils_unittests}"
)

add_executable(
    Run-Main
    main.cpp
    "${utils}"
    "${imgui}"
    "${euler_problems}"
)
target_include_directories(Run-Main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
target_link_libraries(Run-Main PRIVATE glfw)
target_link_libraries(Run-Main PRIVATE glfw OpenGL::GL)
target_compile_definitions(Run-Main PRIVATE GLFW_DLL)
add_custom_command(TARGET Run-Main POST_BUILD
        COMMAND ${CMAKE_COMMAND}  -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:Run-Main>
)


add_executable(
    GLFW-Demo
    glfw_demo.cpp
)

target_link_libraries(GLFW-Demo PRIVATE glfw)
add_custom_command(TARGET GLFW-Demo POST_BUILD
        COMMAND ${CMAKE_COMMAND}  -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:GLFW-Demo>
)

add_executable(
    ImGUI-Demo
    im_gui_demo.cpp
    "${imgui}"
)

target_include_directories(ImGUI-Demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
target_link_libraries(ImGUI-Demo PRIVATE glfw)
target_link_libraries(ImGUI-Demo PRIVATE glfw OpenGL::GL)
target_compile_definitions(ImGUI-Demo PRIVATE GLFW_DLL)
add_custom_command(TARGET ImGUI-Demo POST_BUILD
        COMMAND ${CMAKE_COMMAND}  -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:ImGUI-Demo>
)
