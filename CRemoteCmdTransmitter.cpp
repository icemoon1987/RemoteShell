
#include <iostream>
#include <arpa/inet.h>

#include "CRemoteCmdTransmitter.h"
#include "PacketStructures.h"
#include "config.h"

using namespace std;

/**#define CREMOTE_CMD_TRANSMITTER_DEBUG_OUTPUT */

static string g_moduleName = "CRemoteCmdTransmitter";

CRemoteCmdTransmitter::CRemoteCmdTransmitter(CSocketInterface &socket, const short id) : m_socket(socket), m_id(id)
{
	if( m_socket.IsServer() )
	{
		m_socket.SetNonBlocking(m_socket.GetListenSock());
	}
	m_socket.Prepare();
}


CRemoteCmdTransmitter::~CRemoteCmdTransmitter()
{
}


bool CRemoteCmdTransmitter::IsReady(void)
{
	if(m_socket.IsConnected())
	{
		return true;
	}
	else
	{
#ifdef CREMOTE_CMD_TRANSMITTER_DEBUG_OUTPUT
		if(m_socket.IsServer())
		{
			cout << "waiting for incoming connection..." << endl;
		}
		else
		{
			cout << "connecting sever..." << endl;
		}
#endif

		m_socket.Connect();

		if(m_socket.IsConnected())
		{
			m_socket.SetNonBlocking(m_socket.GetDataSock());

			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}


/**
 * @brief	Cycly receive command packet
 *
 * @return	>0 Received successfully
 *			0 Partial packet received
 *			-1 Connection is broken
 *			-2 no data to receive
 *			-3 Packet length exceed the max length
 */
int CRemoteCmdTransmitter::RecvCmdPacket(void)
{
	static char recvBuf[CMD_PACKET_MAX_LEN * 2];
	static int leftLen = 0;

	int nowPktLen = 0;
	int ret = 0;

	/** If we have the packet head, we can retrive the command length, and we can calculate the expected packet length of this packet*/
	if( leftLen >= CMD_PACKET_HEAD_LEN)
	{
		nowPktLen = ntohs(*(short*)&(recvBuf[CMD_PACKET_CMDLEN_POS])) + CMD_PACKET_HEAD_LEN;

		/** if the packet length is wrong, we reset the buffer */
		if(nowPktLen > CMD_PACKET_MAX_LEN)
		{
			memset(recvBuf, 0, sizeof(recvBuf));
			leftLen = 0;

			return -3;
		}
	}
	else
	{
		/** If we don't have enough information to decide the packet length, we recv once more */
		ret = m_socket.Recv(recvBuf + leftLen, sizeof(recvBuf) - leftLen);

		if(ret == 0 || ret == -1)
		{
			/** Failed to receive or the connection closed by peer */
			return -1;
		}
		else if( ret == -2 )
		{
			/** No data to receive */
			return -2;
		}
		else
		{
			leftLen += ret;
			return 0;
		}
	}

	/** If we have the whole packet, we extract it */
	memset(m_pktBuf, 0 , sizeof(m_pktBuf));
			
	if( leftLen >= nowPktLen)
	{
		memmove(m_pktBuf, recvBuf, nowPktLen);
		memmove(recvBuf, recvBuf + nowPktLen, leftLen - nowPktLen);

		leftLen = leftLen - nowPktLen;

		return nowPktLen;
	}
	else
	{
		/** If we don't have the whole packet, we receive once more */
		
		ret = m_socket.Recv(recvBuf + leftLen, sizeof(recvBuf) - leftLen);

		if(ret == 0 || ret == -1)
		{
			/** Failed to receive or the connection closed by peer */
			return -1;
		}
		else if(ret == -2)
		{
			/** No data to receive */
			return -2;
		}
		else
		{
			leftLen += ret;
			return 0;
		}
	}

	return 0;
}


void CRemoteCmdTransmitter::ParsePacket(CmdPacket &pkt)
{
	uint32_t offset = 0;

	pkt.protocolVer = m_pktBuf[offset];
	offset += 1;

	pkt.id = ntohs(*(short*)&(m_pktBuf[offset]));
	offset += 2;

	pkt.dir = m_pktBuf[offset];
	offset += 1;

	pkt.reserved = ntohs(*(short*)&(m_pktBuf[offset])); 
	offset += 2;

	pkt.cmdLen = ntohs(*(short*)&(m_pktBuf[offset])); 
	offset += 2;

	pkt.cmd = &m_pktBuf[offset];

	return;
}


void CRemoteCmdTransmitter::ComposePacket(CmdPacket &pkt)
{
	uint32_t offset = 0;

	memset(m_pktBuf, 0, sizeof(m_pktBuf));

	m_pktBuf[offset] = pkt.protocolVer;
	offset += 1;

	*(short*)&m_pktBuf[offset] = htons(pkt.id);
	offset += 2;

	m_pktBuf[offset] = pkt.dir;
	offset += 1;

	*(short*)&m_pktBuf[offset] = pkt.reserved;
	offset += 2;
	
	*(short*)&m_pktBuf[offset] = htons(pkt.cmdLen);
	offset += 2;

	memmove(&m_pktBuf[offset], (void*)pkt.cmd, pkt.cmdLen);

	return;
}

/**
 * @brief	Receive a command
 *
 * @param	cmd	Buffer for the incoming command
 *
 * @return	0	success
 *			-1	Connection is not establised
 *			-2	Partial packet received
 *			-3	Packet with wrong length received
 *			-4	Packet with wrong protocol version received
 *			-5	Packet with wrong id received
 *			-6	Packet with wrong dir received
 */
int CRemoteCmdTransmitter::RecvCmd(string &cmd)
{
	CmdPacket pkt;

	if( !IsReady() )
	{
		cout << g_moduleName << ": " <<  "Not ready to receive command! " << "File: "  __FILE__ << " Line: " << __LINE__ << endl;

		cmd = "";
		return -1;
	}

	int ret = 0;

	ret = RecvCmdPacket();

	if( ret == -1 )
	{
		/** If connection is broken, we return a enpty string  */
		cout << g_moduleName << ": " <<  "The connection is broken " << endl;
		cmd = "";
		return -1;
	}

	if( ret == 0 ) 
	{ 
		/** If partial packet received, we return a empty string*/
#ifdef CREMOTE_CMD_TRANSMITTER_DEBUG_OUTPUT
		cout << g_moduleName << ": " <<  "Partial packet received" << endl;
#endif

		cmd = "";
		return -2;
	}

	if( ret == -2 ) { 
		/** If no data received, we return a empty string*/
#ifdef CREMOTE_CMD_TRANSMITTER_DEBUG_OUTPUT
		cout << g_moduleName << ": " <<  "No data received " << endl;
#endif
		cmd = "";
		return -2;
	}


	if( ret == -3 )
	{
		/** if we received some packet with a wrong length */
		cout << g_moduleName << ": " <<  "Received packet with wrong length " << endl;
		cmd = "";
		return -3;
	}

	/** Successfully received the command packet, parse it! */
#ifdef CREMOTE_CMD_TRANSMITTER_DEBUG_OUTPUT
	cout << g_moduleName << ": " <<  "Received command packet!" << endl;
#endif

#ifdef CREMOTE_CMD_TRANSMITTER_DEBUG_OUTPUT
	cout << "Received Len = " << ret << endl;
	cout << "Received data: " << endl;

	cout << hex;
	for(int i = 0 ; i < ret ; ++i)
	{
		cout << (int)m_pktBuf[i] << "	";
		if( (i + 1) % 10 == 0 )
		{
			cout << endl;
		}
	}
	cout << endl;
	cout << dec;
#endif

	ParsePacket(pkt);

	if(pkt.protocolVer != PROTOCOL_VERSION)
	{
		cout << g_moduleName << ": " <<  "wrong protocol version! received="<< pkt.protocolVer << " expected= " << PROTOCOL_VERSION<< endl;
		cmd = "";
		return -4;
	}
	else if(pkt.id != m_id)
	{
		cout << g_moduleName << ": " <<  "wrong protocol id! received="<< pkt.id << " expected= " << m_id << endl;
		cmd = "";
		return -5;
	}
	else if(pkt.dir != CMD_DIR_FROM_PEER)
	{
		cout << g_moduleName << ": " <<  "wrong direction! received="<< pkt.dir << " expected= " << CMD_DIR_FROM_PEER << endl;
		cmd = "";
		return -6;
	}

	cmd = pkt.cmd;
	return 0;

}


int CRemoteCmdTransmitter::SendResult(const std::string &result)
{

	cout << "Sending result:" << result.c_str() << endl;
	cout << "Sending result Length:" << result.length() << endl;

	if( !IsReady() )
	{
		cout << g_moduleName << ": " <<  "Not ready to send result! " << "File: "  __FILE__ << " Line: " << __LINE__ << endl;

		return -1;
	}

	CmdPacket pkt;

	pkt.protocolVer = PROTOCOL_VERSION;
	pkt.id = m_id;
	pkt.dir = CMD_DIR_TO_PEER;
	pkt.reserved = 0;
	pkt.cmdLen = result.length();
	pkt.cmd = result.c_str();

	ComposePacket(pkt);

	m_socket.Send(m_pktBuf, CMD_PACKET_HEAD_LEN + pkt.cmdLen);
	
	return 0;
}



