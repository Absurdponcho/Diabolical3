#pragma once
#include <assert.h>
#include <string>

void CheckCrash(const char* Func, const char* Reason, int Line) ;

template<typename... Args>
void CheckCrashFormat(const char* Func, const char* Reason, int Line, Args... args)
{
	const int BufferSize = 4096 * 2;
	char Buffer[BufferSize] = {};

	snprintf(Buffer, BufferSize, Reason, args...);
	CheckCrash(Func, Buffer, Line);
}

#define Check(x) if (!(x)){CheckCrash(__FUNCTION__, #x, __LINE__);}; 
#define Checkf(x, f, ...) if (!(x)){CheckCrashFormat(__FUNCTION__, f, __LINE__, __VA_ARGS__);}; 



