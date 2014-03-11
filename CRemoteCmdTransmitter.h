#ifndef CREMOTECMDTRANSMITTER_H
#define CREMOTECMDTRANSMITTER_H

#include <string>
#include "CCmdInterface.h"
#include "CSocketInterface.h"
#include "PacketStructures.h"

class CRemoteCmdTransmitter: public CCmdInterface
{
	private:
		short m_id;
		CSocketInterface &m_socket;
		char m_pktBuf[CMD_PACKET_MAX_LEN];
	
	public:
		CRemoteCmdTransmitter(CSocketInterface &socket, const short id);
		~CRemoteCmdTransmitter();

	private:
		int RecvCmdPacket(void);
		void ParsePacket(CmdPacket &pkt);
		void ComposePacket(CmdPacket &pkt);

	public:
		bool IsReady(void);
		int RecvCmd(std::string &cmd);
		int SendResult(const std::string &result);
};


#endif

