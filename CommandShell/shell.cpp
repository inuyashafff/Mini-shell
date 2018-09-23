//Copyright:
//License:
//Author:
//Time:
//Description:

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <vector>

#include "command.h"

Shell::Shell() {
	char buffer[MAX_PATH_LEN];
	getcwd(buffer, sizeof(buffer));
	current_directory_=buffer;
}




void Shell::HandleEXIT(){
	std::cout << "myShell exits." << std::endl;
}

void Shell::HandleCD(Command command){     // 只看第二个参数， 忽略后面的， 没找到
	if (chdir(command[1].c_str()) == 0) {
		char buffer[MAX_PATH_LEN];
	    getcwd(buffer, sizeof(buffer));
		current_directory_=buffer;
	} else {
		std::cout << "cd Error!" << std::endl;
	}
}

bool Shell::isLegalVarname(std::string varname) {
	bool ans=true;
	std::string::iterator it;
	for (it=varname.begin(); it!=varname.end(); ++it) {
		if ((*it>='a' &&  *it<='z') ||
			(*it>='A' &&  *it<='Z') ||
			(*it>='0' &&  *it<='9') ||
			(*it=='_')) {
			continue;
		} else {
			ans=false;
			break;
		}
	}

	return ans;
}


void Shell::HandleSET(Command command){
	if (command[1]=="") {
		std::cout << "Illegal! You need to use ""set var value""" << std::endl;
	} else {
		if (isLegalVarname(command[1])) {
			std::map<std::string,std::string>::iterator it;
			it = var_table_.find(command[1]);
			if (it == var_table_.end()) {
				var_table_.insert ( std::pair<std::string,std::string>(command[1],command[2]) );
			} else {
				it->second = command[2];
			}
		}
		else {
			std::cout << "Illegal Varname!" << std::endl;
		}

	}

}

void Shell::HandleEXPORT(Command command){   // 只看第二个参数， 忽略后面的， 没找到
	if (command[1]=="") {
		std::cout << "Illegal! You need to use ""export var""" << std::endl;
	} else {
		std::map<std::string,std::string>::iterator it;
		it = var_table_.find(command[1]);
		if (it == var_table_.end()) {
			std::cout << "There is no variable called "<< command[1] << std::endl;
		} else {
			//存在env则替换，否则加上
			if ( setenv(it->first.c_str(), it->second.c_str(),1) !=0) {
				perror("export error");
			}
			
		}
	}

}


int Shell::CheckBuiltin(Command command) {
	std::string command_name = command.GetCommandName();
	if (command_name=="exit") {
		HandleEXIT();
		return 0;
	} else if (command_name=="cd") {
		HandleCD(command);
		return 1;
	} else if (command_name=="set") {
		HandleSET(command);
		return 1;
	} else if (command_name=="export") {
		HandleEXPORT(command);
		return 1;
	} else {
		return 2;
	}

}

void Shell::Loop(){
	while (true){
		//init()
		std::cout << "myShell:" << current_directory_ << " $";
		fflush(stdout);

		if (parser_.ReadLine() == 0){                       //if encount EOF , exit
			std::cout << std::endl <<"myShell exits." << std::endl;
			break;
		}  
			
		if (parser_.isEmptyLine())                   // if enter, continue
			continue;

		// 数组


		std::vector<std::vector<char*> > parse_result;
		//char* parse_result [MAX_ARG_NUM];
		if (parser_.ParseLine(parse_result,var_table_)==0) {
			std::cout << "Command Syntax Error! Can Not Parse!" << std::endl;
			continue;
		}

		// 循环添加
		command_num_=parse_result.size();

		if (command_num_==1) {
			Command temp_command(parse_result[0]);
			command_=temp_command;

			//有几个 有一个，按照原计划

			//检查是否为builtin，是的话则执行相应程序 并continue   0是exit，1是其他builtin，2不是builtin
			int check_builtin_result= CheckBuiltin(command_);
			if (check_builtin_result == 0) {
				break; 
			} else if (check_builtin_result == 1){
				continue;
			}

			command_.Execute();

		} else {
			for (int i=0; i<command_num_; i++) {
				Command temp_command(parse_result[i]);
				command_list_.push_back(temp_command);
			}

			//检查合法性
				//符合各式
				//不能是内件命令
			//
			if (command_list_[0].input_filename_!=NULL){
				command_list_[0].input_descriptor_= open(command_list_[0].input_filename_, O_RDONLY);
				dup2(command_list_[0].input_descriptor_,0);
			}

			if (command_list_[command_num_-1].output_filename_!=NULL){
				command_list_[command_num_-1].output_descriptor_ = open(command_list_[command_num_-1].output_filename_, O_WRONLY | O_CREAT| O_TRUNC, 0666);
				dup2(command_list_[command_num_-1].output_descriptor_,1);
			}

			int fds[2];
			for(int i=0; i<command_num_; i++) {
				if (i<command_num_-1) {
					pipe(fds);
					command_list_[i].output_descriptor_ = fds[1];
					command_list_[i+1].input_descriptor_ = fds[0];
				}
				
				command_list_[i].ExecuteList(i,command_num_);

				if ((command_list_[i].input_descriptor_) != 0)
					close(command_list_[i].input_descriptor_);

				if ((command_list_[i].output_descriptor_) != 1)
					close(command_list_[i].output_descriptor_);


			}

			command_list_.clear();




		}


			          
	}

}

