//Log.h
#pragma once

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#define LOGI(...) fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n")
#define LOGW(...) fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n")
#define LOGE(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n")

#endif //LOG_H_
