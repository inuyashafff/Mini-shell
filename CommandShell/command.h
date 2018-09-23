//Copyright:
//License:
//Author:
//Time:
//Description:

#ifndef COMMANDSHELL_COMMAND_H_
#define COMMANDSHELL_COMMAND_H_


#define MAX_ARG_NUM 200

#include <vector>
#include <string>

class Command {
  public:
	Command();
	Command(std::vector<char*> args);
	//~command();
	void Execute();
	std::string GetCommandName();
	std::string operator[](int i);

	Command(const Command & rhs);
	Command & operator=(const Command & rhs);
	~Command();

	void ExecuteList(int i,int command_num);
	void ForkToExecuteList(int i, int command_num);
	  int input_descriptor_;
  	int output_descriptor_;
  	int error_descriptor_;

  	char* input_filename_;
  	char* output_filename_;
  	char* error_filename_;

  private:
  	//std::vector<std::string> arguments_;
  	
  	char* arguments_[MAX_ARG_NUM];



  	bool CheckSlash(std::string str);
  	bool CheckFileInDir(std::string path, std::string filename);

  	bool FindInDir();
  	bool FindInPath();
  	void ForkToExecute();

};

#endif //COMMANDSHELL_COMMAND_H_
