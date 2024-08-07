#pragma once

extern "C" {
#include<libavformat/avformat.h>
}
class SAudioFormat
{
public:

	int sampleRate()const { return this->m_sampleRate; }
	void setSampleRate(int sampleRate) { this->m_sampleRate = sampleRate; }

	AVSampleFormat sampleFormat()const { return this->m_sampleFormat; }
	void setSampleFormat(AVSampleFormat sampleFormat) { this->m_sampleFormat = sampleFormat; }

	int sampleChannels()const { return this->m_channels; }
	void setSampleChannels(int sampleChannels) { this->m_channels = sampleChannels; }
private:
	int m_sampleRate{ 48000 };
	AVSampleFormat m_sampleFormat{ AVSampleFormat::AV_SAMPLE_FMT_S16};
	int m_channels{ 2 };

};
