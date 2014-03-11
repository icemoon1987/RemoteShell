#ifndef CSOCKETINTERFACE_H
#define CSOCKETINTERFACE_H

#include <string>

class CSocketInterface
{
	public:
		virtual void SetSrcAddr(const std::string srcAddr) = 0;
		virtual void SetSrcPort(const short port) = 0;

		virtual void SetDstAddr(const std::string srcAddr) = 0;
		virtual void SetDstPort(const short port) = 0;

		virtual std::string	GetSrcAddr(void) = 0;
		virtual short	GetSrcPort(void) = 0;

		virtual std::string	GetDstAddr(void) = 0;
		virtual short	GetDstPort(void) = 0;

	public:
		virtual int GetListenSock(void) = 0;
		virtual int GetDataSock(void) = 0;

	public:
		virtual int SetBlocking(int sock) = 0;
		virtual int SetNonBlocking(int sock) = 0;
		virtual int SetReUseAddress(int sock) = 0;

	public:
		virtual int Prepare(void) = 0;
		virtual int Connect(void) = 0;
		virtual int Close(void) = 0;
		virtual bool IsConnected(void) = 0;
		virtual bool IsServer(void) = 0;

	public:
		virtual int Recv(char * recvBuf, const int len) = 0;
		virtual int Send(const char * sendBuf, const int len) = 0;

};


#endif
