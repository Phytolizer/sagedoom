#pragma once

#define MAJOR_VERSION 1
#define MINOR_VERSION 10
#define VERSION (MAJOR_VERSION * 100 + MINOR_VERSION)

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#define MAJOR_VERSION_STR STRINGIZE(MAJOR_VERSION)
#define MINOR_VERSION_STR STRINGIZE(MINOR_VERSION)
