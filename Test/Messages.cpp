/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Messages.h"
#include <Windows.h>
#undef min
#undef max
#include <cstdio>

// Caller owns the return value
static char* _FormatStr(const char* fmt, va_list args)
{
	size_t size;
#pragma warning(suppress:28719)    // 28719
	size = vsnprintf(nullptr, 0, fmt, args);

	char* message = new char[size + 1u];
	vsnprintf(message, size + 1u, fmt, args);
	message[size] = '\0';

	return message;
}

// Caller owns the return value
char* FormatStr(const char* fmt, ...)
{
	va_list args;
	__crt_va_start(args, fmt);
	char* message = _FormatStr(fmt, args);
	__crt_va_end(args);
	return message;
}

static void _Message(const char* title, unsigned int flags, const char* fmt, va_list args)
{
	const char* message = _FormatStr(fmt, args);
	if (IsDebuggerPresent())
		OutputDebugStringA(message);
	else
		MessageBoxA(0, message, title, flags);
	delete message;
}

void Info(const char* fmt, ...)
{
	va_list args;
	__crt_va_start(args, fmt);
	_Message("Info", MB_OK | MB_ICONINFORMATION, fmt, args);
	__crt_va_end(args);
}

void Warning(const char* fmt, ...)
{
	va_list args;
	__crt_va_start(args, fmt);
	_Message("Warning", MB_OK | MB_ICONWARNING, fmt, args);
	__crt_va_end(args);
	if (IsDebuggerPresent())
		DebugBreak();
}

void Error(const char* fmt, ...)
{
	va_list args;
	__crt_va_start(args, fmt);
	_Message("Error", MB_OK | MB_ICONEXCLAMATION, fmt, args);
	__crt_va_end(args);
	if (IsDebuggerPresent())
		DebugBreak();
}

void Fatal(char* fmt, ...)
{
	va_list args;
	__crt_va_start(args, fmt);
	_Message("Error", MB_OK | MB_ICONEXCLAMATION, fmt, args);
	__crt_va_end(args);
	if (IsDebuggerPresent())
		DebugBreak();
	else
		ExitProcess(0);
}

void Assert(bool expression)
{
	if (expression)
		return;
	if (IsDebuggerPresent())
		DebugBreak();
}

void Assert(bool expression, char* fmt, ...)
{
	if (expression)
		return;
	va_list args;
	__crt_va_start(args, fmt);
	_Message("Error", MB_OK | MB_ICONEXCLAMATION, fmt, args);
	__crt_va_end(args);
	if (IsDebuggerPresent())
		DebugBreak();
}

void AssertFatal(bool expression)
{
	if (expression)
		return;
	if (IsDebuggerPresent())
		DebugBreak();
	else
		ExitProcess(0);
}

void AssertFatal(bool expression, const char* fmt, ...)
{
	if (expression)
		return;
	va_list args;
	__crt_va_start(args, fmt);
	_Message("Error", MB_OK | MB_ICONEXCLAMATION, fmt, args);
	__crt_va_end(args);
	if (IsDebuggerPresent())
		DebugBreak();
	else
		ExitProcess(0);
}
