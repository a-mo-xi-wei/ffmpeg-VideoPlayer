#include<QApplication>
#include<QWidget>
#include"SFFmpeg.h"
#include"SLogger.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QWidget w;
	w.show();

	SFFmpeg ffmpeg;
	if (!ffmpeg.open("E:/Res/VideoRes/video.mp4")) {
		LOG_ERROR("open faild");
		return 0;
	}
	while (true) {
		AVPacket pkt = ffmpeg.read();
		if (pkt.size == 0) {
			LOG_ERROR("ffmpeg.read failed !");
			return 0;
		}

		AVFrame* frame = ffmpeg.decode(&pkt);
		if (!frame) {
			LOG_ERROR("decode failed");
		}
		else LOG_INFO("success");
		av_packet_unref(&pkt);
	}
	
	  


	ffmpeg.close();

	return a.exec();
}
