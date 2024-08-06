#include "VideoThread.h"
#include"SLogger.h"
#include"SFFmpeg.h"

void VideoThread::start()
{
	stop();
	std::lock_guard<std::mutex>lock(this->m_mutex);
	m_thread = new(std::nothrow) std::thread(&VideoThread::run, this);
	if (!m_thread) {
		LOG_ERROR("thread start failed");
		return;
	}
	this->m_abort = false;
}

void VideoThread::stop()
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	if (this->m_thread) {
		this->m_abort = false;
		//等待线程关闭
		if (this->m_thread->joinable())
			this->m_thread->join();
		//释放线程
		delete this->m_thread;
		this->m_thread = nullptr;

	}
}

void VideoThread::run()
{
	while (!this->m_abort) {
		//LOG_INFO("thread %p ", std::this_thread::get_id());
		AVPacket pkt = SFFmpeg::instance().read();
		//读取包失败
		if (pkt.size == 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(10));
			continue;
		}
		if (pkt.stream_index != SFFmpeg::instance().videoIndex()) {
			av_packet_unref(&pkt);
			continue;
		}
		//解码
		AVFrame* frame = SFFmpeg::instance().decode(&pkt);
		if (!frame) {
			av_packet_unref(&pkt);
			continue;
		}
		av_packet_unref(&pkt);
		//睡眠 每一帧小号的时间（毫秒）
		std::this_thread::sleep_for(std::chrono::milliseconds(
		static_cast<long long>(1000 / SFFmpeg::instance().fps())));

	}
}
