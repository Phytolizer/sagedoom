#pragma once

#define MAJOR_VERSION 1
#define MINOR_VERSION 1

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

#define MAJOR_VERSION_STR STRINGIZE(MAJOR_VERSION)
#define MINOR_VERSION_STR STRINGIZE(MINOR_VERSION)
