#pragma once

#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <functional>
#include <mutex>
#include <tuple>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#endif

#ifdef __unix__
#include <curses.h>
#endif