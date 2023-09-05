#pragma once

#include <netinet/in.h>
#include "output.hpp"


class RtpOutput : public Output
{
public:
	RtpOutput(VideoOptions const *options);
	~RtpOutput();

protected:
	void outputBuffer(void *mem, size_t size, int64_t timestamp_us, uint32_t flags) override;

private:
	int fd_;
	sockaddr_in saddr_;
	const sockaddr *saddr_ptr_;
	socklen_t sockaddr_in_size_;
};
