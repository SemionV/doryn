#pragma once

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#endif

#include "base/base.h"