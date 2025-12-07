#pragma once
#include "precompile_header.h"

namespace guis {

	// Sets GLFW window hints appropriate to the current platform and returns
	// a GLSL version string suitable for ImGui's OpenGL3 backend.
	const char* imgui_glfw_setup_for_current_platform();

} // namespace guis

