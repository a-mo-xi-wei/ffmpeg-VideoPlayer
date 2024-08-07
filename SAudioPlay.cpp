#include "SAudioPlay.h"
#include<QAudioFormat>
#include<QAudioSink>
#include<QMediaDevices>
#include<mutex>

class CSAudioPlay : public SAudioPlay
{
	std::mutex m_mutex;
	std::unique_ptr<QAudioSink>m_aSink;
	QIODevice* m_ioDevice;
public:

	bool start() override
	{
		stop();
		std::lock_guard<std::mutex>lock(this->m_mutex);
		//音频格式
		QAudioFormat fmt;
		fmt.setSampleRate(audioFormat().sampleRate());	//采样率
		fmt.setSampleFormat(QAudioFormat::SampleFormat::Int16);//采样格式
		fmt.setChannelCount(audioFormat().sampleChannels());	//通道数
		
		//创建音频输出
		this->m_aSink.reset(new QAudioSink(
			QMediaDevices::defaultAudioOutput(), fmt));
		this->m_ioDevice = this->m_aSink->start();

		return true;
	}
	bool stop() override
	{
		std::lock_guard<std::mutex>lock(this->m_mutex);
		if (m_aSink) {
			this->m_aSink->stop();
			this->m_aSink->reset();
			this->m_ioDevice = nullptr;
		}
		return true;
	}
	bool play(bool isPlay) override
	{
		std::lock_guard<std::mutex>lock(this->m_mutex);
		if (!this->m_aSink) {
			return false;
		}
		if (!isPlay) {
			this->m_aSink->resume();
		}
		else {
			this->m_aSink->suspend();
		}
		return true;
	}
	bool write(const char* data, int size) override
	{
		std::lock_guard<std::mutex>lock(this->m_mutex);
		if (!this->m_ioDevice) {
			return false;
		}
		this->m_ioDevice->write(data, size);

		return false;
	}
	int64_t bytesFree() override
	{
		std::lock_guard<std::mutex>lock(this->m_mutex);
		if (!this->m_aSink) {
			return 0;
		}
		return this->m_aSink->bytesFree();
	}
};

SAudioPlay* SAudioPlay::instance()
{  
	static CSAudioPlay ins;
	return &ins;
}
