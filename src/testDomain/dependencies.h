#pragma once

#include <iostream>

#ifdef _WIN32
#include <conio.h>
#endif

#ifdef __unix__
#include <curses.h>
#endif

#include "base/base.h"