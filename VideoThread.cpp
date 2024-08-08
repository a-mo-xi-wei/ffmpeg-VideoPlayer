#include "VideoThread.h"
#include"SLogger.h"
#include"SFFmpeg.h"
#include"SAudioPlay.h"
#include<list>
#include<queue>

static std::queue<AVPacket,std::list<AVPacket>>videoPkts;	//视频缓冲队列
static int apts = 0;

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
		this->m_abort = true;
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
	char buf[10000] = { 0 };
	while (!this->m_abort) {
		//LOG_INFO("thread %p ", std::this_thread::get_id());

		if (!SFFmpeg::instance().isPlay()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		//判断视频队列是否为空
		while(!videoPkts.empty()) {
			AVPacket pkt = videoPkts.front();
			//获取 pkt 的pts
			auto pts = SFFmpeg::instance().packetPts(pkt);
			//如果视频的pts大于了音频的pts 
			if (pts > apts)break;
			SFFmpeg::instance().decode(&pkt);
			av_packet_unref(&pkt);
			videoPkts.pop();
		}

		//判断空闲空间是否足够
		if (SAudioPlay::instance()->bytesFree() < 10000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		AVPacket pkt = SFFmpeg::instance().read();
		//读取包失败
		if (pkt.size == 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		
		//播放音视频
		if (pkt.stream_index == SFFmpeg::instance().audioIndex()) {
			//解码
			apts = SFFmpeg::instance().decode(&pkt);
			av_packet_unref(&pkt);
			int out_cnt = 0;
			if (SFFmpeg::instance().toPCM(buf, &out_cnt)) {
				SAudioPlay::instance()->write(buf, out_cnt);
			}
			else {
				LOG_ERROR("to pcm failed");
			}
			continue;
		}
		else if (pkt.stream_index == SFFmpeg::instance().videoIndex()) {
			//解码
			//SFFmpeg::instance().decode(&pkt);
			//av_packet_unref(&pkt);
			videoPkts.push(pkt);
			continue;
		}
		//睡眠 每一帧消耗的时间（毫秒）
		//std::this_thread::sleep_for(std::chrono::milliseconds(
		//static_cast<long long>(1000 / SFFmpeg::instance().fps())));

	}
}
