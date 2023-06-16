#pragma once

#ifdef MESSAGELIBRARY_EXPORTS
#define MESSAGELIBRARY_API __declspec(dllexport)
#else
#define MESSAGELIBRARY_API __declspec(dllimport)
#endif

#include <string>

extern "C"
{
    MESSAGELIBRARY_API void message(const std::string text);
}