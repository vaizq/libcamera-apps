#include "rtp_output.hpp"
#include <iostream>

RtpOutput::RtpOutput(VideoOptions const *options) : Output(options)
{
	char protocol[4];
	int start, end, a, b, c, d, port;
	if (sscanf(options->output.c_str(), "%3s://%n%d.%d.%d.%d%n:%d", protocol, &start, &a, &b, &c, &d, &end, &port) != 6)
		throw std::runtime_error("bad network address " + options->output);
	m_remote_address = options->output.substr(start, end - start);
	m_remote_port = port;

	LOG(2, "Sending to address: " + m_remote_address + ":" + std::to_string(m_remote_port));

	m_sess = m_ctx.create_session(m_remote_address, "0.0.0.0");

	if (strcasecmp(options->codec.c_str(), "h264") == 0)
	{
		int flags = RCE_SEND_ONLY;
		m_stream = m_sess->create_stream(m_remote_port, RTP_FORMAT_H264, flags);
	}
	else
	{
		int flags = RCE_FRAGMENT_GENERIC | RCE_SEND_ONLY;
		m_stream = m_sess->create_stream(m_remote_port, RTP_FORMAT_GENERIC, flags);
	}
}

RtpOutput::~RtpOutput()
{
	m_ctx.destroy_session(m_sess);
}

void RtpOutput::outputBuffer(void *mem, size_t size, int64_t /*timestamp_us*/, uint32_t /*flags*/)
{
	if (m_stream->push_frame((uint8_t*)mem, size, RTP_NO_FLAGS) != RTP_OK)
	{
		LOG_ERROR("Failed to send frame!");
	}
	else 
	{
		std::cout << "sent " << size << " bytes" << std::endl;
	}
}
