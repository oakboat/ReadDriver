#pragma once
#ifdef DEBUG
#define dbg(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, __VA_ARGS__)
#else
#define dbg(fmt, ...)
#endif