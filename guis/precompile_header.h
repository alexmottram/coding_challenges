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

// Optional third-party GUI headers (commented out to avoid hard dependency during compile)
// If you want real ImGui usage, uncomment and ensure include path is available.
#include <imgui.h>
#include <GLFW/glfw3.h>
