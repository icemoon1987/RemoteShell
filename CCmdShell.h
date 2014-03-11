#ifndef CCMDSHELL_H
#define CCMDSHELL_H

#include <string>
#include "CCmdInterface.h"

class CCmdShell
{
	private:
		CCmdInterface &m_cmdIf;

	public:
		CCmdShell(CCmdInterface &cmdIf);
		~CCmdShell();

	public:
		int RunLocalProcess(std::string path, char *argv[]);

};


#endif
