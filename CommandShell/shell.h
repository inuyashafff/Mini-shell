//Copyright:
//License:
//Author:
//Time:
//Description:

#ifndef COMMANDSHELL_SHELL_H_
#define COMMANDSHELL_SHELL_H_

#include <vector>
#include <string>
#include <map>

#include "parser.h"
#include "command.h"


#define MAX_PATH_LEN 200


class Shell {
  public:
  	Shell();
  	void Loop();

  private:
  	Parser parser_;
  	Command command_;
    std::vector<Command> command_list_;
    int command_num_;
  	
  	std::string current_directory_;
  	std::map<std::string, std::string> var_table_;

  	void HandleEXIT();
  	void HandleCD(Command command);
  	bool isLegalVarname(std::string varname);
  	void HandleSET(Command command);
  	void HandleEXPORT(Command command);

  	int CheckBuiltin(Command command);

};


#endif  //COMMANDSHELL_SHELL_H_

