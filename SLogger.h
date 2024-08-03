#ifndef SLOGGER_H
#define SLOGGER_H

#include<mutex>

#define _LOG_(level,fmt,...)	SLogger::getInstance().log(level, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define LOG_INFO(fmt,...)		_LOG_(SLogLevel::INFO,fmt,##__VA_ARGS__)
#define LOG_DEBUG(fmt,...)		_LOG_(SLogLevel::DEBUG,fmt,##__VA_ARGS__)
#define LOG_WARNING(fmt,...)	_LOG_(SLogLevel::WARNING,fmt,##__VA_ARGS__)
#define LOG_ERROR(fmt,...)		_LOG_(SLogLevel::ERROR,fmt,##__VA_ARGS__)


class SLogLevel
{
public:
	enum Level
	{
		INFO,	//信息
		DEBUG,	//调试
		WARNING,//警告
		ERROR	//错误
	};

	inline static const char* toString(SLogLevel::Level level) {
		switch (level)
		{
		case SLogLevel::INFO:return "INFO";
		case SLogLevel::DEBUG:return "DEBUG";
		case SLogLevel::WARNING:return "WARNING";
		case SLogLevel::ERROR:return "ERROR";
		default:
			break;
		}
		return "UnKnown";
	}

};
class SLogger
{
public:
	static SLogger& getInstance();
	void log(SLogLevel::Level level, const char* func, 
		const char* file, int line, const char* fmt, ...);
	void setDefaultLevel(SLogLevel::Level level) { m_level = level; }
private:
	SLogger() = default;
	~SLogger() = default;
	SLogger(const SLogger&) = delete;				// 禁用拷贝构造
	SLogger operator=(const SLogger&) = delete;		// 禁用赋值运算符
	SLogger(SLogger&&) = delete;					// 禁用移动构造（C++11）
	SLogger operator=(SLogger&&) = delete;			// 禁用移动赋值（C++11）


	std::mutex m_mutex;
	SLogLevel::Level m_level{ SLogLevel::INFO };
};
#endif // !SLOGGER_H
