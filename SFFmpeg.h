#ifndef SFFMPEG_H
#define SFFMPEG_H
#include<string>
#include<mutex>

extern "C" {
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include"libavutil/avutil.h"
#include"libswscale/swscale.h"
}
class SFFmpeg
{
public:
	SFFmpeg();
	bool open(const std::string& filename);
	bool isOpen()const;
	void close();
	std::string errorString()const;
	uint32_t duration()const { return m_totalMs; }

	inline int videoIndex()const { 
		//printf("m_videoIndex : %d", this->m_videoIndex); 
		return this->m_videoIndex; 
	}
	inline int audioIndex()const { return this->m_audioIndex; }

	AVPacket read();
	AVFrame* decode(const AVPacket* pkt);

	bool toRGB(char* out, int outWidth, int outHeight);
private:
	AVCodecContext* StreamCodecContext(int index);

private:
	AVFormatContext* m_fmtCtx{};
	mutable std::mutex m_mutex;
	char m_errBuf[AV_ERROR_MAX_STRING_SIZE]{};

	uint32_t m_totalMs{};	//视频时长(ms)

	int m_videoIndex{ -1 };
	int m_audioIndex{ -1 };

	AVCodecContext* m_codecCtx[AVMEDIA_TYPE_NB]{};
	AVFrame* m_yuvFrame{};

	SwsContext* m_swsCtx{};	//缩放上下文
};

#endif // !SFFMPEG_H
