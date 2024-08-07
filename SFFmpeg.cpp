#include "SFFmpeg.h"
#include"SLogger.h"
SFFmpeg::SFFmpeg()
{
}

SFFmpeg& SFFmpeg::instance()
{
	static SFFmpeg ffmpeg;
	return ffmpeg;
}

static inline double q2d(AVRational a) {
	if (a.den == 0)return 0.0;
	return a.num / (double)a.den;
}

bool SFFmpeg::open(const std::string& filename)
{
	if (isOpen())close();
	std::lock_guard<std::mutex>lock(this->m_mutex);
	int ret = avformat_open_input(&this->m_fmtCtx, filename.data(), nullptr, nullptr);
	if (ret != 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		LOG_ERROR("avformat_open_input error : %s", this->m_errBuf);
		return false;
	}
	
	//读取流信息
	ret = avformat_find_stream_info(this->m_fmtCtx, nullptr);
	if (ret < 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		LOG_ERROR("avformat_open_input error : %s", this->m_errBuf);
		avformat_close_input(&this->m_fmtCtx);
		return false;
	}

	//获取视频时长
	m_totalMs = (double)this->m_fmtCtx->duration / AV_TIME_BASE * 1000;

	//查找视频流
	this->m_videoIndex = av_find_best_stream(this->m_fmtCtx,
		AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (this->m_videoIndex < 0) {
		if (this->m_videoIndex == AVERROR_STREAM_NOT_FOUND) {
			LOG_WARNING("video stream not found !");
		}
		else if (this->m_videoIndex == AVERROR_DECODER_NOT_FOUND) {
			LOG_WARNING("video decoder not found !");
		}
		av_strerror(this->m_videoIndex, this->m_errBuf,
			sizeof(this->m_errBuf));
	}
	else {
		this->m_codecCtx[this->m_videoIndex] = 
			StreamCodecContext(this->m_videoIndex);
		this->m_fps = q2d(this->m_fmtCtx->streams[this->m_videoIndex]->avg_frame_rate);
	}

	//查找音频流
	this->m_audioIndex = av_find_best_stream(this->m_fmtCtx, 
		AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	if (this->m_audioIndex < 0) {
		if (this->m_audioIndex == AVERROR_STREAM_NOT_FOUND) {
			LOG_WARNING("audio stream not found !");
		}
		else if (this->m_audioIndex == AVERROR_DECODER_NOT_FOUND) {
			LOG_WARNING("audio decoder not found !");
		}
		av_strerror(this->m_audioIndex, this->m_errBuf,
			sizeof(this->m_errBuf));
	}
	else
	{
		this->m_codecCtx[this->m_audioIndex] =
			StreamCodecContext(this->m_audioIndex);
		this->m_audioFmt.setSampleRate(this->m_codecCtx[this->m_audioIndex]->sample_rate);
		this->m_audioFmt.setSampleChannels(this->m_codecCtx[this->m_audioIndex]->ch_layout.nb_channels);
		this->m_audioFmt.setSampleFormat(this->m_codecCtx[this->m_audioIndex]->sample_fmt);
			
		LOG_DEBUG("audio sample-reat : %d sample-format : %d channel count : %d ",
			this->m_audioFmt.sampleRate(),this->m_audioFmt.sampleFormat(), this->m_audioFmt.sampleChannels());
	}
	return true;
}

bool SFFmpeg::isOpen() const
{
	return !!this->m_fmtCtx;
}

void SFFmpeg::close()
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	//关闭打开的文件
	if (isOpen()) {
		avformat_close_input(&this->m_fmtCtx); 
	}
	//释放解码器上下文
	for (size_t i = 0; i < AVMEDIA_TYPE_NB; i++)
	{
		if (this->m_codecCtx[i]) {
			avcodec_free_context(&this->m_codecCtx[i]);
		}
	}
	if (this->m_yuvFrame) {
		av_frame_free(&this->m_yuvFrame);
	}
	if (this->m_pcmFrame) {
		av_frame_free(&this->m_pcmFrame);
	}

	swr_free(&this->m_swrCtx);
	sws_freeContext(this->m_swsCtx);
	this->m_swsCtx = nullptr;
}

std::string SFFmpeg::errorString() const
{ 
	std::lock_guard<std::mutex>lock(this->m_mutex);
	return std::string(this->m_errBuf);
}

void SFFmpeg::aspectSize(int* w, int* h)
{
	switch (this->m_aspectMode)
	{
	case SFFmpeg::AspectNone:
		this->m_rational = { 0 };
		break;
	case SFFmpeg::Aspect4_3:
		this->m_rational = { 4 , 3};
		break;
	case SFFmpeg::Aspect16_9:
		this->m_rational = { 16 , 9};
		break;
	}
	if (this->m_aspectMode != AspectNone && this->m_rational.num != 0 &&
		this->m_rational.den != 0) {
		*h = FFMIN(*w / q2d(this->m_rational), *h);
		*w = FFMIN(*h * q2d(this->m_rational), *w);
	}


}

double SFFmpeg::packetPts(const AVPacket& pkt)
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	if (isOpen() || pkt.size == 0)return 0.0;
	return this->m_pts =  pkt.pts * q2d(this->m_fmtCtx->streams[pkt.stream_index]->time_base) * 1000;
	 
}

AVPacket SFFmpeg::read()
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	AVPacket pkt{};
	if (!isOpen()) {
		strncpy(this->m_errBuf, "AVFormatContext is null ,file not open !", sizeof(this->m_errBuf));
		LOG_WARNING(this->m_errBuf);
		return pkt;
	}

	int ret = av_read_frame(this->m_fmtCtx, &pkt);
	if (ret < 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		//LOG_WARNING(this->m_errBuf);
		return pkt;
	}
	return pkt;
}

int SFFmpeg::decode(const AVPacket* pkt)
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	if (!isOpen()) {
		LOG_ERROR("未打开文件");
		return -1;
	}

	if (!this->m_yuvFrame) {
		this->m_yuvFrame = av_frame_alloc();
	}
	if (!this->m_pcmFrame) {
		this->m_pcmFrame = av_frame_alloc();
	}

	int ret = avcodec_send_packet(this->m_codecCtx[pkt->stream_index], pkt);
	if (ret != 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		if (ret == AVERROR(EAGAIN)) {
			LOG_DEBUG("AVERROR(EAGAIN)");
		}
		else if (ret == AVERROR_EOF) {
			LOG_DEBUG("AVERROR_EOF");
		}
		else LOG_ERROR("other error : %s",this->m_errBuf);
	}

	AVFrame* frame = nullptr;
	if (pkt->stream_index == this->m_audioIndex)
		frame = this->m_pcmFrame;
	else
		frame = this->m_yuvFrame;

	ret = avcodec_receive_frame(this->m_codecCtx[pkt->stream_index], frame);
	if (ret != 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		if (ret == AVERROR(EAGAIN)) {
			LOG_DEBUG("AVERROR(EAGAIN)");
		}
		else if (ret == AVERROR_EOF) {
			LOG_DEBUG("AVERROR_EOF");
		}
		else LOG_DEBUG(this->m_errBuf);
		return -1;
	}
	double pts = frame->pts * q2d(this->m_fmtCtx->streams[pkt->stream_index]->time_base) * 1000;

	if (pkt->stream_index == this->m_videoIndex) {
		//获取当前播放时长
		this->m_pts = pts;
		if (this->m_aspectMode == AspectMode::AspectOriginal) {
			this->m_rational = av_d2q(
				(static_cast<double>(this->m_yuvFrame->width) /
					frame->height), 1920);
		}
	}
	return pts;
}

bool SFFmpeg::toRGB(char* out, int outWidth, int outHeight)
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	if (!isOpen() || !this->m_yuvFrame || !this->m_yuvFrame->data[0])
		return false;

 	this->m_swsCtx = sws_getCachedContext(this->m_swsCtx,
		this->m_yuvFrame->width, this->m_yuvFrame->height,
		(AVPixelFormat)this->m_yuvFrame->format,outWidth, outHeight,
		AV_PIX_FMT_RGBA, SWS_BICUBIC,nullptr, nullptr, nullptr);
	if (!this->m_swsCtx) {
		LOG_WARNING("sws_getCachedContext failed");
		return false;
	}
	uint8_t* data[AV_NUM_DATA_POINTERS] = { (uint8_t*)out };
	int linesize[AV_NUM_DATA_POINTERS] = {outWidth * 4};

	int h = sws_scale(this->m_swsCtx, this->m_yuvFrame->data,
		this->m_yuvFrame->linesize,0,this->m_yuvFrame->height,
		data,linesize);
	if (h <= 0) {
		LOG_ERROR("sws_scale failed !");
	}
	//LOG_INFO("h : %d", h);
	return true;
}

bool SFFmpeg::toPCM(char* out, int* outSize)
{
	std::lock_guard<std::mutex>lock(this->m_mutex);
	if (!isOpen() || !m_pcmFrame || !m_pcmFrame->data[0])
		return false;
	if (!this->m_swrCtx) {
		int ret = swr_alloc_set_opts2(&m_swrCtx,
			&this->m_pcmFrame->ch_layout,
			AV_SAMPLE_FMT_S16,
			this->m_audioFmt.sampleRate(),
			&this->m_pcmFrame->ch_layout,
			(AVSampleFormat)this->m_pcmFrame->format,
			this->m_pcmFrame->sample_rate,
			0, nullptr);
		if (ret != 0) {
			av_strerror(ret,this->m_errBuf, sizeof(this->m_errBuf));
			LOG_ERROR("swr_alloc_set_opts2 failed %s ", this->m_errBuf);
			return false;
		}

		ret = swr_init(this->m_swrCtx);
		if (ret < 0)return -1;
	}
	uint8_t* data[1] = { (uint8_t*)out };
	//获取输出采样数
	auto out_cnt = swr_get_out_samples(this->m_swrCtx, this->m_pcmFrame->nb_samples);
	//重采样
	int ret = swr_convert(this->m_swrCtx,data,out_cnt,this->m_pcmFrame->data, this->m_pcmFrame->nb_samples);
	if (ret < 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		LOG_ERROR("swr_convert failed %s ", this->m_errBuf);
		return false;
	}
	//获取输出的数据大小
	ret = av_samples_get_buffer_size(nullptr,
		this->m_pcmFrame->ch_layout.nb_channels, 
		this->m_pcmFrame->nb_samples, 
		AVSampleFormat::AV_SAMPLE_FMT_S16, 0);
	if (ret < 0) {
		av_strerror(ret, this->m_errBuf, sizeof(this->m_errBuf));
		LOG_ERROR("swr_convert failed %s ", this->m_errBuf);
		return false;
	}
	*outSize = ret;
	return true;
}

AVCodecContext* SFFmpeg::StreamCodecContext(int index)
{
	//打开解码器上下文
	AVCodecContext* codecCtx = nullptr;
	do {
		codecCtx = avcodec_alloc_context3(nullptr);
		if (!codecCtx) {
			
			LOG_WARNING("codecCtx 分配内存失败");
			break;
		}
		int ret = avcodec_parameters_to_context(codecCtx,
			this->m_fmtCtx->streams[index]->codecpar);
		if (ret < 0) {
			LOG_WARNING("codecCtx 分配编码器参数失败");
			break;
		}
		const AVCodec* codec = avcodec_find_decoder(codecCtx->codec_id);
		if (!codec) {
			LOG_WARNING("decoder not found !");
			break;
		}

		ret = avcodec_open2(codecCtx, codec, nullptr);
		if (ret < 0) {
			LOG_WARNING("解码器打开失败");
			break;
		}
		return codecCtx;
	} while (false);
	avformat_close_input(&this->m_fmtCtx);
	return nullptr;
}
