#include"SAudioFormat.h"

class SAudioPlay
{
protected:
	SAudioPlay(){};

public:
	static SAudioPlay* instance();

	virtual bool start() = 0;

	virtual bool stop() = 0;

	virtual bool play(bool isPlay) = 0;

	virtual bool write(const char* data, int size) = 0;

	virtual int64_t bytesFree() = 0;

	SAudioFormat audioFormat()const { return this->m_audioFmt; }
	void setAudioFormat(const SAudioFormat& fmt) { this->m_audioFmt = fmt; }
private:
	SAudioFormat m_audioFmt;

};

