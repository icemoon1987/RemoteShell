#ifndef _CONFIGFILEEXTRACTOR_H
#define _CONFIGFILEEXTRACTOR_H

#include <vector>
#include <map>
#include <string>
#include <fstream>

enum KeywordType
{
	KEY_TYPE_INT,
	KEY_TYPE_DOUBLE,
	KEY_TYPE_STRING,
	KEY_TYPE_BOOL,
	KEY_TYPE_ERROR
};

typedef std::pair<std::string,enum KeywordType> Keyword;


class CConfigFileParser
{
	private:
		std::map<std::string, std::string> m_params;

	private:
		int m_paraNum;

	private:
		std::string KeytypeToString(const enum KeywordType type);
		void * GetValue(const std::string key);
		int StringToInt(const std::string value);
		double StringToDouble(const std::string value);
		bool StringToBool(std::string value);
		std::string &TrimSpaces(std::string &s);
		std::string &TrimComments(std::string &s, const char commToken);
		int PhaseLine(std::string line, const char delim);
		int StoreParameter(const std::string key, const std::string value);

	public:
		CConfigFileParser();
		~CConfigFileParser();
		
	public:
		void DumpParameters(void);
		void Clear(void);

	public:
		int ExtractFile(const std::string filePath, const char delim = '=', const char commToken = '#');
		int GetValueInt(std::string key);
		double GetValueDouble(std::string key);
		std::string GetValueString(std::string key);
		bool GetValueBool(std::string key);

};

#endif


