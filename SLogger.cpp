#include "SLogger.h"
#include<iostream>
#include<stdarg.h>

SLogger& SLogger::getInstance()
{
	static SLogger logger;
	return logger;
}

void SLogger::log(SLogLevel::Level level, const char* func, const char* file, int line, const char* fmt, ...)
{
	if (level < m_level)return;
	std::lock_guard<std::mutex>lock(m_mutex);  
	char buf[128];
	va_list va;
	va_start(va, fmt);
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	printf("[%s] %s %s(%s:%d)\n", SLogLevel::toString(level), buf, file, func, line);

}
