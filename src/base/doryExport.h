#pragma once

#ifdef DORY_EXPORT
#define DORY_API __declspec(dllexport)
#else
#define DORY_API __declspec(dllimport)
#endif