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

set(guis
    "${imgui}"
    guis/guis.h
    guis/precompile_header.cpp utils/precompile_header.h
    guis/imgui_glfw_setup.cpp guis/imgui_glfw_setup.h
    guis/gui_manager.cpp guis/gui_manager.h
    guis/widgets/gui_logger.cpp guis/widgets/gui_logger.h
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
    GuiTest
    guis/demos/simple_logger_demo.cpp
    "${guis}"
    "${utils}"
)

target_include_directories(GuiTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
target_link_libraries(GuiTest PRIVATE glfw)
target_link_libraries(GuiTest PRIVATE glfw OpenGL::GL)
target_compile_definitions(GuiTest PRIVATE GLFW_DLL)

add_executable(
    CounterDemo
    guis/demos/multiple_logger_demo.cpp
    "${guis}"
    "${utils}"
)

target_include_directories(CounterDemo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
target_link_libraries(CounterDemo PRIVATE glfw)
target_link_libraries(CounterDemo PRIVATE glfw OpenGL::GL)
target_compile_definitions(CounterDemo PRIVATE GLFW_DLL)
