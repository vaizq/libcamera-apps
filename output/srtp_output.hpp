#pragma once

#include <uvgrtp/lib.hh>
#include "output.hpp"


class SrtpOutput : public Output
{
public:
	SrtpOutput(VideoOptions const *options);
	~SrtpOutput();

protected:
	void outputBuffer(void *mem, size_t size, int64_t timestamp_us, uint32_t flags) override;

private:
	std::string m_remote_address;
	int m_remote_port;
	int m_local_port;
	uvgrtp::context m_ctx;
	uvgrtp::session *m_sess;
	uvgrtp::media_stream *m_stream;
};
