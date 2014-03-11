#ifndef CTCPSOCKET_H
#define CTCPSOCKET_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <fcntl.h>

#include "CSocketInterface.h"

class CTcpSocket: public CSocketInterface
{
	private:
		bool m_isServer;

		int m_sock;
		int m_listenSock;

		struct sockaddr_in m_srcAddr;
		struct sockaddr_in m_dstAddr;

		bool m_isConnect;

	public:
		CTcpSocket(bool isServer);
		~CTcpSocket();
		
	public:
		void SetSrcAddr(const std::string srcAddr); 
		void SetSrcPort(const short port);

		void SetDstAddr(const std::string dstAddr);
		void SetDstPort(const short port);

		std::string	GetSrcAddr(void);
		short	GetSrcPort(void);

		std::string	GetDstAddr(void);
		short	GetDstPort(void);

	public:
		int GetListenSock(void);
		int GetDataSock(void);

	public:
		int SetBlocking(int sock);
		int SetNonBlocking(int sock);
		int SetReUseAddress(int sock);

	public:
		int Prepare(void);
		int Connect(void);
		int Close(void);
		bool IsConnected(void);
		bool IsServer(void);

	public:
		int Recv(char * recvBuf, const int len);
		int Send(const char * sendBuf, const int len);
};


#endif
