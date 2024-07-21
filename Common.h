#pragma once


#if _WIN32 || _WIN64
#if _WIN32
#define PTR_SIZE UINT32
#define LARGEST_ADDRESS 0x7FFFFFFF
#else
#define PTR_SIZE UINT64
#define LARGEST_ADDRESS 0x7FFFFFFFFFFFFFFF
#endif
#endif