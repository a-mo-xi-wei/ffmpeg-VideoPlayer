#pragma once
#include<thread>
#include<mutex>

class VideoThread
{
public:
	void start();
	void stop();
	void run();//线程入口函数

private:
	std::thread* m_thread{};
	std::mutex m_mutex;
	bool m_abort{false};	//线程是否终止
};