#pragma once

#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <functional>
#include <mutex>
#include <tuple>
#include <utility>
#include <type_traits>
#include <array>
#include <cmath>
#include <string>
#include <filesystem>
#include <fstream>
#include <optional>
#include <list>
#include <string_view>
#include <variant>
#include <algorithm>
#include <random>
#include <future>
#include <any>
#include <cassert>
#include <shared_mutex>
#include <set>

#include <nlohmann/json.hpp>
#include <ryml.hpp>
#include <boost/dll.hpp>
#include <refl.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/null_sink.h>

#ifdef __unix__
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

#ifdef WIN32
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>
