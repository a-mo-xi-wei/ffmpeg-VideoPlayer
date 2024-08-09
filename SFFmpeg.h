#ifndef SFFMPEG_H
#define SFFMPEG_H
#include<string>
#include<mutex>
#include"SAudioFormat.h"
extern "C" {
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include"libavutil/avutil.h"
#include"libswscale/swscale.h"
#include"libswresample/swresample.h"
}
class SFFmpeg
{
public:
	enum AspectMode
	{
		AspectNone,		//自由缩放
		AspectOriginal,	//原始比例
		Aspect4_3,		//4 : 3
		Aspect16_9		//16 : 9
	};


public:
	SFFmpeg();

	static SFFmpeg& instance();

	bool open(const std::string& filename);
	bool isOpen()const;
	void close();

	std::string errorString()const;
	uint32_t duration()const { return m_totalMs; }
	double fps() { return this->m_fps; }
	uint32_t position() { return this->m_pts; }

	inline int videoIndex()const { 
		//printf("m_videoIndex : %d", this->m_videoIndex); 
		return this->m_videoIndex; 
	}
	inline int audioIndex()const { return this->m_audioIndex; }
	
	inline int volume()const { return this->m_volume; }
	inline void setVolume(const double& volume) { this->m_volume = volume; }

	bool isPlay()const { return this->m_isPlay; }
	void setPlay(bool play) { this->m_isPlay = play; }

	AspectMode aspectMode()const { return this->m_aspectMode; }
	void setAspectMode(AspectMode mode) { this->m_aspectMode = mode; }

	void aspectSize(int* w, int* h);

	SAudioFormat audioFormat()const { return this->m_audioFmt; }
	void setAudioFormat(const SAudioFormat& fmt) { this->m_audioFmt = fmt; }

	double packetPts(const AVPacket& pkt);

	AVPacket read();
	int decode(const AVPacket* pkt);

	bool toRGB(char* out, int outWidth, int outHeight);
	bool toPCM(char* out, int* outSize);



private:
	AVCodecContext* StreamCodecContext(int index);

private:
	AVFormatContext* m_fmtCtx{};
	mutable std::mutex m_mutex;
	char m_errBuf[AV_ERROR_MAX_STRING_SIZE]{};

	uint32_t m_totalMs{};	//视频时长(ms)
	double m_fps = 0.0;		//视频帧率
	double m_pts = 0.0;		//当前播放时长
	int m_volume = 50;	//当前声音大小
	bool m_isPlay{ false };

	//缩放模式
	AspectMode m_aspectMode{ AspectMode::AspectOriginal };
	//缩放比例
	AVRational m_rational{ 0,0 };

	int m_videoIndex{ -1 };
	int m_audioIndex{ -1 };

	AVCodecContext* m_codecCtx[AVMEDIA_TYPE_NB]{};
	AVFrame* m_yuvFrame{};
	AVFrame* m_pcmFrame{};

	SwsContext* m_swsCtx{};	//缩放上下文
	SwrContext* m_swrCtx{};	//音频重采样

	SAudioFormat m_audioFmt;

};

#endif // !SFFMPEG_H
