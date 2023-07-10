#pragma once

#include <vector>
#include <chrono>
#include <thread>

#include <iostream>
#include <functional>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#endif

#ifdef __unix__
#include <curses.h>
#endif