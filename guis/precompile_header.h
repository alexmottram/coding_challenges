#pragma once

// Common STL headers used by the GUI subsystem
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <iostream>
#include <chrono>
#include <cstdio>

// Optional third-party GUI headers (these are required by the ImGui/GLFW-based GUI code).
// If you build targets that don't use ImGui/GLFW, you can define NO_IMGUI to avoid the dependency
// and comment these out or wrap them behind a project-wide macro.
#ifndef NO_IMGUI
#include <imgui.h>
#include <GLFW/glfw3.h>
#endif
