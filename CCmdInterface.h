#ifndef CCMDINTERFACE_H
#define CCMDINTERFACE_H

#include <string>

class CCmdInterface
{
	public:
		virtual bool IsReady(void) = 0;
		virtual int RecvCmd(std::string &cmd) = 0;
		virtual int SendResult(const std::string &result) = 0;
};

#endif
