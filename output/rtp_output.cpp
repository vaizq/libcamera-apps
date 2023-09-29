#include "rtp_output.hpp"
#include <iostream>


static void recv_hook(uvgrtp::frame::rtcp_receiver_report* frame) {
    std::cout << "RTCP receiver report! ----------"       << std::endl;

    for (auto& block : frame->report_blocks)
    {
        std::cout << "ssrc: "       << block.ssrc     << std::endl;
        std::cout << "fraction: "   << block.fraction << std::endl;
        std::cout << "lost: "       << block.lost     << std::endl;
        std::cout << "last_seq: "   << block.last_seq << std::endl;
        std::cout << "jitter: "     << block.jitter   << std::endl;
        std::cout << "lsr: "        << block.lsr      << std::endl;
        std::cout << "dlsr (jiffies): "  << uvgrtp::clock::jiffies_to_ms(block.dlsr)
                  << std::endl << std::endl;
    }

    /* RTCP frames can be deallocated using delete */
    delete frame;
}

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

	int flags = RCE_RTCP;
	if (strcasecmp(options->codec.c_str(), "h264") == 0)
	{
		m_stream = m_sess->create_stream("0.0.0.0", m_remote_port, RTP_FORMAT_H264, flags);
	}
	else
	{
		int flags = RCE_FRAGMENT_GENERIC;
		m_stream = m_sess->create_stream("0.0.0.0", m_remote_port, RTP_FORMAT_GENERIC, flags);
	}

	if (m_stream->get_rtcp()->install_receiver_hook(recv_hook) != RTP_OK)
		throw std::runtime_error("failed to install receive hook");
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
