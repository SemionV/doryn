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

#include "nlohmann/json.hpp"
#include "ryml.hpp"
#include "refl-cpp/include/refl-cpp/refl.hpp"
#ifdef USE_SPDLOG
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/null_sink.h"
#endif

#ifndef USE_FMT
namespace fmt
{
    template<typename T, typename... Ts>
    std::string format(T&& str, Ts... params)
    {
        return std::string{ std::forward<T>(str) };
    }
}
#endif
