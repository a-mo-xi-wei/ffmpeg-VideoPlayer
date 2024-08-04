#include<QApplication>
#include"SFFmpeg.h"
#include"SLogger.h"
#include"SPlay.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);

	SPlay s;
	s.show();

	return a.exec();

	SFFmpeg ffmpeg;
	if (!ffmpeg.open("E:/Res/VideoRes/video.mp4")) {
		LOG_ERROR("open faild");
		return 0;
	}

	char* rgbBuf = new char[640 * 480 * 4];
	while (true) {
		AVPacket pkt = ffmpeg.read();
		if (pkt.size == 0) {
			LOG_ERROR("ffmpeg.read failed !");
			return 0;
		}
		//printf("pkt.stream_index : %d ffmpeg.videoIndex() : %d", 
		//	pkt.stream_index , ffmpeg.videoIndex());
		if (pkt.stream_index != ffmpeg.videoIndex()) {
			continue;
		}
		AVFrame* frame = ffmpeg.decode(&pkt);
		if (!frame) {
			LOG_ERROR("decode failed");
			continue;
		}
		//LOG_INFO("success");
		if (ffmpeg.toRGB(rgbBuf, 640, 480)) {
		}
		av_packet_unref(&pkt);
	}
	
	  


	ffmpeg.close();

	return a.exec();
}
