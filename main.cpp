#include <iostream>
#include <string>
#include "CCmdShell.h"
#include "CTcpSocket.h"
#include "CSocketInterface.h"
#include "CRemoteCmdTransmitter.h"
#include "CCmdInterface.h"
#include "CConfigFileParser.h"
#include "config.h"

using namespace std;

string g_version = "0.0.2";

int main( int argc, char *argv[])
{
	if(argc > 1 && !strcmp(argv[1], "--version"))
	{
		cout << g_version << endl;
		return 0;
	}

	if(argc < 3)
	{
		cout << "Usage: RemoteShell path-to-a-executable-file config-file-path parameters" << endl;
		return 1;
	}

	cout << "Remote Shell Stated" << endl;
	cout << "Parsing config file: " << argv[2] << "...";

	CConfigFileParser conf;
	CTcpSocket server(true);

	conf.ExtractFile(argv[2], '=', '#');

	int id = conf.GetValueInt("id");
	int selfPort = conf.GetValueInt("self_port");
	string selfIP = conf.GetValueString("self_ip");

	cout << "Finish" << endl;

	cout << "Communication Parameters:" << endl;
	cout << "id = " << id << endl;
	cout << "Listen IP = " << selfIP << endl;
	cout << "Listen Port = " << selfPort << endl;

	server.SetSrcAddr(selfIP.c_str());
	server.SetSrcPort(selfPort);

	CRemoteCmdTransmitter cmd(server, id);

	CCmdShell cmdShell(cmd);

	cmdShell.RunLocalProcess(argv[1], &argv[2]);

	return 1;
}



