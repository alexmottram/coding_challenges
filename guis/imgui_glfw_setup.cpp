#include "imgui_glfw_setup.h"

namespace guis {

    const char* imgui_glfw_setup_for_current_platform() {
    #if defined(__APPLE__)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1); // GL_TRUE replacement
        return "#version 150";
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        return "#version 130";
    #endif
    }

} // namespace guis
