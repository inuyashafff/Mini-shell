//Copyright:
//License:
//Author:
//Time:
//Description:


#ifndef COMMANDSHELL_PARSER_H_
#define COMMANDSHELL_PARSER_H_

#include <string>
#include <vector>
#include <map>
#include "command.h"


class Parser {
  public:
	Parser();
	//~Parser();

	int ReadLine();
	bool isEmptyLine();

	void Parse(std::vector<char*> &result, const std::map<std::string,std::string> var_table, std::string sub_command_line);
	int ParseLine(std::vector<std::vector<char*> > &result, const std::map<std::string,std::string> var_table);

  private:
  	std::string command_line_;
  	void SkipSpace(std::string::iterator &it);
	std::string GetVarName(std::string str, std::string::iterator it);
	std::string VariableSubstitution(std::string str, const std::map<std::string,std::string> var_table);

};

#endif //COMMANDSHELL_PARSER_H_
