#ifndef PACKETSTRUCTURES_H
#define PACKETSTRUCTURES_H

typedef struct __CmdPacket
{
	uint8_t protocolVer;
	uint16_t id;
	uint8_t dir;
	uint16_t reserved;
	uint16_t cmdLen;

	const char * cmd; 
}CmdPacket;

#define CMD_PACKET_HEAD_LEN 8
#define CMD_PACKET_MAX_CMD_LEN 2048

#define CMD_PACKET_MAX_LEN (CMD_PACKET_HEAD_LEN + CMD_PACKET_MAX_CMD_LEN)

#define CMD_PACKET_CMDLEN_POS 6 

#define CMD_DIR_FROM_PEER 0
#define CMD_DIR_TO_PEER 1

#endif
