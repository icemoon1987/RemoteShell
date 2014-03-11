#include <iostream>
#include <string>
#include "CTcpSocket.h"

/**#define CTCPSOCKET_DEBUG_OUTPUT */

using namespace std;

static string g_moduleName = "CTcpSocket";

CTcpSocket::CTcpSocket(bool isServer)
:m_isServer(isServer), m_isConnect(false)
{
	if(isServer)
	{
		m_listenSock = socket(AF_INET, SOCK_STREAM, 0);
		m_sock = 0;
	}
	else
	{
		m_listenSock = 0;
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
	}

	m_srcAddr.sin_family = AF_INET;
	m_srcAddr.sin_port = 0;
	m_srcAddr.sin_addr.s_addr = 0;

	m_dstAddr.sin_family = AF_INET;
	m_dstAddr.sin_port = 0;
	m_dstAddr.sin_addr.s_addr = 0;

	m_isConnect = false;

	return;
}


CTcpSocket::~CTcpSocket()
{
	if(m_isServer)
	{
		close(m_listenSock);

		if(m_isConnect)
		{
			close(m_sock);
		}
	}
	else
	{
		close(m_sock);
	}

	return;
}


void CTcpSocket::SetSrcAddr(const std::string srcAddr)
{
	/** TODO: Check Input Address */
	m_srcAddr.sin_addr.s_addr = inet_addr(srcAddr.c_str());
	return;
}


void CTcpSocket::SetSrcPort(const short port)
{
	m_srcAddr.sin_port = htons(port);
	return;
}


void CTcpSocket::SetDstAddr(const std::string dstAddr)
{
	/** TODO: Check Input Address */
	m_dstAddr.sin_addr.s_addr = inet_addr(dstAddr.c_str());
	return;
}


void CTcpSocket::SetDstPort(const short port)
{
	m_dstAddr.sin_port = htons(port);
	return;
}


string	CTcpSocket::GetSrcAddr(void)
{
	return inet_ntoa(m_srcAddr.sin_addr);
}


short	CTcpSocket::GetSrcPort(void)
{
	return ntohs(m_srcAddr.sin_port);
}


string	CTcpSocket::GetDstAddr(void)
{
	return inet_ntoa(m_dstAddr.sin_addr);
}


short	CTcpSocket::GetDstPort(void)
{
	return ntohs(m_dstAddr.sin_port);
}


int CTcpSocket::GetListenSock(void)
{
	return m_listenSock;
}


int CTcpSocket::GetDataSock(void)
{
	return m_sock;
}


int CTcpSocket::SetBlocking(int sock)
{
	int flags = fcntl(sock, F_GETFL, 0);

	if (flags < 0)
	{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
		cout << g_moduleName << ": "  <<  "fcntl failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
		return -1;
	}

	flags &= ~O_NONBLOCK;

	if (fcntl(sock, F_SETFL, flags) == -1)
	{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
		cout << g_moduleName << ": "  <<  "fcntl failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
		return -1;
	}

	return 0;
}


int CTcpSocket::SetNonBlocking(int sock)
{
	int flags = fcntl(sock, F_GETFL, 0);

	if (flags < 0)
	{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
		cout << g_moduleName << ": "  <<  "fcntl failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
		return -1;
	}

	flags |= O_NONBLOCK;

	if (fcntl(sock, F_SETFL, flags) == -1)
	{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
		cout << g_moduleName << ": "  <<  "fcntl failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
		return -1;
	}

	return 0;
}


int CTcpSocket::SetReUseAddress(int sock)
{
	const int on = 1;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0) 
	{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
		cout << g_moduleName << ": "  <<  "setsockopt failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
		return -1;
	}

	return 0;
}


int CTcpSocket::Prepare(void)
{
	if(m_isServer)
	{
		/** As a Server, We listen on a port, wait for an connection.*/
		socklen_t len = sizeof(struct sockaddr);
		if( bind(m_listenSock, (struct sockaddr*)&m_srcAddr, len) == -1)
		{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
			cout << g_moduleName << ": "  <<  "bind failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
			return -1;
		}

		if( listen(m_listenSock, 5) == -1 )
		{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
			cout << g_moduleName << ": "  <<  "listen failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
			return -1;

		}

	}
	else
	{
		/** As a Client, We only need to do the bind */
		socklen_t len = sizeof(struct sockaddr);
		if( bind(m_sock, (struct sockaddr*)&m_srcAddr, len) == -1)
		{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
			cout << g_moduleName << ": "  <<  "bind failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
			return -1;
		}

	}

	return 0;
}


int CTcpSocket::Connect(void)
{
	socklen_t len = sizeof(struct sockaddr);
	if(m_isServer)
	{
		/** On server side, we wait for connection */
		m_sock = accept(m_listenSock, (struct sockaddr*)&m_dstAddr, &len);

		if( m_sock == -1)
		{
			if( errno == EAGAIN )
			{
				/** In non-blocking mode, this means there's no incoming connection request */
				m_isConnect = false;
			}
			else
			{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
				cout << g_moduleName << ": "  <<  "Accept failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
				m_isConnect = false;
			}

			return -1;
		}
		else
		{
			m_isConnect = true;
		}
	}
	else
	{
		/** On Client side, we connect to the server */
		if( connect(m_sock, (struct sockaddr*)&m_dstAddr, sizeof(struct sockaddr)) == -1)
		{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
			cout << g_moduleName << ": "  <<  "Connect failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
			m_isConnect = false;
			return -1;
		}
		else
		{
			m_isConnect = true;
		}
	}

	return 0;
}


int CTcpSocket::Close(void)
{
	if(m_isServer)
	{
		close(m_listenSock);

		if(m_isConnect)
		{
			close(m_sock);
		}
	}
	else
	{
		close(m_sock);
	}

	m_isConnect = false;

	return 0;
}


bool CTcpSocket::IsConnected(void)
{
	return m_isConnect;
}


/**
 * @brief	Receive data from the data socket
 *
 * @param	recvBuf	Buffer for received data.
 * @param	len		Receive buffer length.
 *
 * @return	>0	Sucessfully received data length
 *			0	Connection closed by peer	
 *			-1	Failed to receive
 *			-2	No data to receive (non-blocking mode)
 */
int CTcpSocket::Recv(char * recvBuf, const int len)
{
	int ret = 0;

	ret = recv(m_sock, recvBuf, len, 0);
	if(ret == -1)
	{
		if( errno == EWOULDBLOCK)
		{
			/** In non-block style, this means there's no data to receive */
			return -2;
		}
		else
		{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
			cout << g_moduleName << ": "  <<  "Recv failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
			m_isConnect = false;
			
			return -1;
		}
	}
	else if( ret == 0 )
	{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
		cout << g_moduleName << ": "  <<  "Connection closed by the peer" << endl;
#endif
		m_isConnect = false;

		return 0;
	}

	return ret;
}


/**
 * @brief	Send data through data socket, will do cycle send automatically
 *
 * @param	sendBuf	Buffer for data to be sent.
 * @param	len		Length of data to be sent.
 *
 * @return	-1	Failed to send data.
 *			0	Success to send data.
 */
int CTcpSocket::Send(const char * sendBuf, const int len)
{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
	cout << "send 1: " << endl;
	cout << "sendBuf = " << sendBuf << endl;
	cout << "len = " << len << endl;
#endif

	int ret = 0;
	int sentLen = 0;

	while( sentLen < len )
	{
		ret = send(m_sock, sendBuf + sentLen , len - sentLen, 0);
		if(ret == -1)
		{
#ifdef CTCPSOCKET_DEBUG_OUTPUT
			cout << g_moduleName << ": "  <<  "Send failed: " << strerror(errno) << "File: "  __FILE__ << " Line: " << __LINE__ << endl;
#endif
			m_isConnect = false;

			return -1;
		}
		else
		{
			sentLen += ret;
		}
	}
#ifdef CTCPSOCKET_DEBUG_OUTPUT
	cout << "send 2: " << endl;
#endif

	return 0;
}


bool CTcpSocket::IsServer(void)
{
	return m_isServer;
}


