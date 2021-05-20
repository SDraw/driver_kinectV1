#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <sstream>
#include <thread>
#include <chrono>

#include "openvr.h"

#include "SFML/Main.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/GpuPreference.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "pugixml.hpp"
