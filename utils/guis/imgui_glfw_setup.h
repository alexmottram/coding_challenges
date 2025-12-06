#pragma once
// Centralize ImGui + GLFW GLSL selection and GLFW window-hint setup.
// Minimal, header-only helper. Call this after glfwInit() and before creating a GLFW window.

#include <GLFW/glfw3.h>

namespace utils {

    enum class ImgGlfwBackend {
        Auto,
        GL_ES2,
        GL_ES3,
        GL_CORE,
        GL_LEGACY
    };

    // Return a pointer to a static GLSL-version string valid for the lifetime of the program.
    // Side effect: sets GLFW context-related window hints for the next-created window.
    // Requirements: call this after glfwInit() and before glfwCreateWindow() from the thread
    // that initializes/creates GLFW windows.
    // Optional parameters: set_visible and set_resizable control whether the helper also
    // sets GLFW_VISIBLE and GLFW_RESIZABLE hints. They default to true to preserve
    // existing behavior.
    inline const char* imgui_glfw_setup_for_current_platform(ImgGlfwBackend force = ImgGlfwBackend::Auto,
                                                             bool set_visible = true,
                                                             bool set_resizable = true)
    {
#if defined(IMGUI_IMPL_OPENGL_ES2)
        if (force == ImgGlfwBackend::Auto || force == ImgGlfwBackend::GL_ES2) {
            static constexpr const char* glsl_version = "#version 100";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            if (set_visible) glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            if (set_resizable) glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            return glsl_version;
        }
#endif

#if defined(IMGUI_IMPL_OPENGL_ES3)
        if (force == ImgGlfwBackend::Auto || force == ImgGlfwBackend::GL_ES3) {
            static constexpr const char* glsl_version = "#version 300 es";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            if (set_visible) glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            if (set_resizable) glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            return glsl_version;
        }
#endif

#if defined(__APPLE__)
        if (force == ImgGlfwBackend::Auto || force == ImgGlfwBackend::GL_CORE) {
            static constexpr const char* glsl_version = "#version 150";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            if (set_visible) glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            if (set_resizable) glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            return glsl_version;
        }
#endif

        // Default desktop GL path
        if (force == ImgGlfwBackend::Auto || force == ImgGlfwBackend::GL_LEGACY ||
            force == ImgGlfwBackend::GL_CORE) {
            static constexpr const char* glsl_version = "#version 130";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            if (set_visible) glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
            if (set_resizable) glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            return glsl_version;
        }

        static constexpr const char* fallback = "#version 130";
        if (set_visible) glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        if (set_resizable) glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        return fallback;
    }

} // namespace utils
