#pragma once

#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#endif

#ifdef __unix__
#include <curses.h>
#endif