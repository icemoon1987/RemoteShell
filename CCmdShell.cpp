#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <poll.h>
#include "CCmdShell.h"
#include "PacketStructures.h"
#include "config.h"

using namespace std;

/**#define CCMDSHELL_DEBUG_OUTPUT*/

static string g_module_name = "CCmdShell";

CCmdShell::CCmdShell(CCmdInterface &cmdIf)
	: m_cmdIf(cmdIf)
{
}

CCmdShell::~CCmdShell()
{
}


int CCmdShell::RunLocalProcess(std::string path, char *argv[])
{
	int parentFds[2];
	int childFds[2];

	int pid;

	char sendBuf[CMD_PACKET_MAX_CMD_LEN]; 
	//string sendBuf;
	string recvBuf;

	/** Build Pipes */

	if(pipe(parentFds) == -1)
	{
		cout << g_module_name << ": " << "failed to build pipes for parent process!" << endl;
		return -1;
	}

	if(pipe(childFds) == -1)
	{
		cout << g_module_name << ": " << "failed to build pipes for child process!" << endl;
		return -1;
	}

	pid = fork();

	if( pid == -1 )
	{
		cout << g_module_name << ": " << "failed to fork child process!" << endl;
		return -1;
	}
	else if( pid == 0 )
	{
		/** Child */
		close(parentFds[1]);
		close(childFds[0]);

		/** Pass the parameters and exec the child */

		dup2(parentFds[0], STDIN_FILENO);
		dup2(childFds[1], STDOUT_FILENO);

		execv(path.c_str(), argv);

	}
	else
	{
		/** Parent */
		close(parentFds[0]);
		close(childFds[1]);

		/** Poll on the read pipe */

		struct pollfd fds[1];

		fds[0].fd = childFds[0];
		fds[0].events = POLLIN;
	
		cout << "Started child process, waiting for command..." << endl;

		FILE * pf = fdopen(childFds[0], "r");
		string result;
		stringstream ssTem;
		string tem = "";
		vector<string> resultList;


		while(1)
		{
			/** getline(cin, sendBuf);*/
			if(m_cmdIf.IsReady())
			{
				recvBuf.clear();
				if(0 == m_cmdIf.RecvCmd(recvBuf))
				{
					cout << g_module_name << ": receive cmd: " << recvBuf << endl;

					/** Write to child process */
					if(write(parentFds[1], recvBuf.c_str(), recvBuf.length() ) == -1)
					{
						cout << g_module_name << ": failed to write to pipe!" << endl;
						return 1;
					}
					else
					{
						if(write(parentFds[1], "\n", 1 ) == -1)
						{
							cout << g_module_name << ": failed to write to pipe!" << endl;
							return 1;
						}

#ifdef CCMDSHELL_DEBUG_OUTPUT
						cout << g_module_name << ": send cmd to child!" << endl;
#endif
					}
				}
			}

			/** Read from child process */
			int ret = poll(fds, 1, 10);
			
			if(ret > 0)
			{
				ssTem.clear();

				memset(sendBuf, 0 , sizeof(sendBuf));
				int len = read(childFds[0], sendBuf, sizeof(sendBuf) - 1);
				//fgets(sendBuf, sizeof(sendBuf), pf);
				//int n = sizeof(sendBuf);

				//char *p = sendBuf;

				//getline(&p, (size_t *)&n, pf);

				//int len = strlen(sendBuf);

				//getline(cin, sendBuf);

				sendBuf[len]= '\0';

				result = sendBuf;
				ssTem.str(result);

				while(ssTem)
				{
					getline(ssTem, tem);
					if(tem == "" || tem == "\n")
					{
						continue;
					}
					resultList.push_back(tem);
				}
				

				while(!resultList.empty())
				{
					string tem = resultList.front().c_str();
					resultList.erase(resultList.begin());

					int len = tem.length();

#ifdef CCMDSHELL_DEBUG_OUTPUT
					cout << "sendBuf = " << tem.c_str() << endl;
					cout << "sendBuf Len = " << len << endl;
#endif

					m_cmdIf.SendResult(tem.c_str());
#ifdef CCMDSHELL_DEBUG_OUTPUT
					cout << g_module_name << ": send result: " << tem.c_str() << endl;
#endif
				}
			}
			else if( ret == -1 )
			{
				cout << g_module_name <<  ": poll() failed! " << endl;
				return -1;
			}

			usleep(1000 * CYCLE_TIME);
		}
	}

	return 0;
}

