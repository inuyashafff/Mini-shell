//Copyright:
//License:
//Author:
//Time:
//Description:

#include "command.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <iostream>
#include <vector>
#include <string>
#include <cstring>

Command::Command()
	: input_descriptor_(0),
	  output_descriptor_(1),
	  error_descriptor_(2),
	  input_filename_(NULL),
	  output_filename_(NULL),
	  error_filename_(NULL) {
	for (std::size_t i=0; i<MAX_ARG_NUM; i++) 
		arguments_[i]=NULL;
}

Command::Command(std::vector<char*> args) 
	: input_descriptor_(0),
	  output_descriptor_(1),
	  error_descriptor_(2),
	  input_filename_(NULL),
	  output_filename_(NULL),
	  error_filename_(NULL) {
	
	std::size_t arguments_num=0;
	std::vector<char*>::iterator it=args.begin();
	arguments_[0]=args[0];
	arguments_num++;
	++it;


	while (*it!=NULL) {
		if (std::strcmp(*it,"<")==0) {
			if (it!=args.end()-2){
				input_filename_=*(it+1);
				delete[](*it);
				it+=2;
				continue;
			} else {
				delete[](*it);
				++it;
				continue;
			}
		} else if (std::strcmp(*it,">")==0) {
			if (it!=args.end()-2){
				output_filename_=*(it+1);
				delete[](*it);
				it+=2;
				continue;
			} else {
				delete[](*it);
				++it;
				continue;
			}
		} else if (std::strcmp(*it,"2>")==0) {
			if (it!=args.end()-2){
				input_filename_=*(it+1);
				delete[](*it);
				it+=2;
				continue;
			} else {
				delete[](*it);
				++it;
				continue;
			}
		} else {
			arguments_[arguments_num]=*it;
			arguments_num++;
			++it;
		}
	}

	for (std::size_t i=arguments_num; i<MAX_ARG_NUM; i++) 
		arguments_[i]=NULL;


}


Command::Command(const Command & rhs) {

	input_descriptor_=rhs.input_descriptor_;
	output_descriptor_=rhs.output_descriptor_;
	error_descriptor_=rhs.error_descriptor_;


    if (rhs.input_filename_!=NULL) {
        input_filename_= new char [std::strlen(rhs.input_filename_)+1];
        std::strcpy (input_filename_, rhs.input_filename_);
    } else {
    	input_filename_=NULL;
    }
    
    if (rhs.output_filename_!=NULL) {
        output_filename_= new char [std::strlen(rhs.output_filename_)+1];
        std::strcpy (output_filename_, rhs.output_filename_);
    } else {
    	output_filename_=NULL;
    }

    if (rhs.error_filename_!=NULL) {
        error_filename_= new char [std::strlen(rhs.error_filename_)+1];
        std::strcpy (error_filename_, rhs.error_filename_);
    } else {
    	error_filename_=NULL;
    } 


    for (std::size_t i=0; i<MAX_ARG_NUM; i++) {
        if (rhs.arguments_[i]!=NULL) {
            arguments_[i]= new char [std::strlen(rhs.arguments_[i])+1];
            std::strcpy (arguments_[i], rhs.arguments_[i]);
        } else {
            arguments_[i]=NULL;
        }
    }

}

Command & Command::operator=(const Command & rhs) {
        if (input_filename_!=NULL) {
                delete[](input_filename_);
        }
        if (output_filename_!=NULL) {
                delete[](output_filename_);
        }
        if (error_filename_!=NULL) {
                delete[](error_filename_);
        }

	std::size_t i=0;
	while (arguments_[i]!=NULL) {
		delete[](arguments_[i]);
		i++;
	}

	input_descriptor_=rhs.input_descriptor_;
	output_descriptor_=rhs.output_descriptor_;
	error_descriptor_=rhs.error_descriptor_;

    if (rhs.input_filename_!=NULL) {
        input_filename_= new char [std::strlen(rhs.input_filename_)+1];
        std::strcpy (input_filename_, rhs.input_filename_);
    } else {
    	input_filename_=NULL;
    }
    
    if (rhs.output_filename_!=NULL) {
        output_filename_= new char [std::strlen(rhs.output_filename_)+1];
        std::strcpy (output_filename_, rhs.output_filename_);
    } else {
    	output_filename_=NULL;
    }

    if (rhs.error_filename_!=NULL) {
        error_filename_= new char [std::strlen(rhs.error_filename_)+1];
        std::strcpy (error_filename_, rhs.error_filename_);
    } else {
    	error_filename_=NULL;
    } 

	for (std::size_t i=0; i<MAX_ARG_NUM; i++) {
        	if (rhs.arguments_[i]!=NULL) {
            		arguments_[i]= new char [std::strlen(rhs.arguments_[i])+1];
            		std::strcpy (arguments_[i], rhs.arguments_[i]);
        	} else {
            		arguments_[i]=NULL;
        	}
	}
	
	return *this;
}

Command::~Command() {
	if (input_filename_!=NULL) {
		delete[](input_filename_);
	}
	if (output_filename_!=NULL) {
		delete[](output_filename_);
	}
	if (error_filename_!=NULL) {
		delete[](error_filename_);
	}

	std::size_t i=0;
	while (arguments_[i]!=NULL) {
		delete[](arguments_[i]);
		i++;
	}

}



std::string Command::GetCommandName() {
	std::string command_name(arguments_[0]);
	return command_name;
}

std::string Command::operator[](int i) {
	std::string argument;
	if (arguments_[i] != NULL) {
		argument=arguments_[i];
	} 
	return argument;
}


bool Command::CheckSlash(std::string str) {
	std::string slash_str ("/");
	std::size_t found = str.find(slash_str);
	if (found == std::string::npos)
		return false;
	else return true;
}


bool Command::CheckFileInDir(std::string path, std::string filename) {
	DIR *pDir;//定义一个DIR类的指针
  	struct dirent *ent; //定义一个结构体 dirent的指针，dirent结构体见上
  	//char childpath[512] ={0};//定义一个字符数组，用来存放读取的路径
  	pDir=opendir(path.c_str());//  opendir方法打开path目录，并将地址付给pDir指针
  	if (pDir==NULL) {
		return false;
	}
  	while((ent=readdir(pDir))!=NULL){//读取pDir打开的目录，并赋值给ent, 同时判断是否目录为空，不为空则执行循环体
    		if((ent->d_type & DT_REG) || (ent->d_type & DT_LNK)) {//读取 打开目录的文件类型 并与 DT_DIR进行位与运算操作，即如果读取的d_type类型为DT_DIR (=4 表示读取的为目录)
    			if (strcmp(ent->d_name, filename.c_str()) == 0) {
    				closedir(pDir);
    				return true;
    			}
    		}
  	}

  	closedir(pDir);//要有close
  	return false;
}



bool Command::FindInDir() {
	std::string str (arguments_[0]);
	std::size_t pos = str.find_last_of("/");
	return CheckFileInDir(str.substr(0,pos),str.substr(pos+1));
}


bool Command::FindInPath() {
	std::string filename (arguments_[0]);
	char* pPath = getenv ("PATH");
  	if (pPath == NULL) {
  		return false;
  	} 
  	std::string str (pPath);

	std::size_t last_pos = -1;



	while (true) {
		if (last_pos==str.length()-1)
			break;

		size_t next_pos = str.find(":",last_pos+1);
		if (next_pos != std::string::npos) {
			if (CheckFileInDir(str.substr(last_pos+1,next_pos-last_pos-1), filename)) {
				return true;
			}
		} else {
			if (CheckFileInDir(str.substr(last_pos+1,str.length()-last_pos-1), filename)) {
				return true;
			}
			break;

		}

		last_pos=next_pos;
	}

	return false;
			
}
	//判断是否有/
	   //如果有/ 查看当前文件是否有
			//有则执行
	        //无责报错
	   //如果没有/ 查看环境变量时候有
			// y有则执行
	        // 无责报错

void Command::Execute() {
	if ( CheckSlash((*this)[0]) ) {
		if (FindInDir()) {
			ForkToExecute();
			return;
		} else {
			std::cout << "Command " << (*this)[0] << " not found" << std::endl;
		}
	} else {
		if (FindInPath()) {
			ForkToExecute();
			return;
		} else {
			std::cout << "Command " << (*this)[0] << " not found" << std::endl;
		}
	}


}



void Command::ForkToExecute() {



	pid_t pid;
	pid = fork();
	if (pid==-1) {
		perror("Fork Fail!");
		exit(EXIT_FAILURE);
	}
	else {
		if (pid==0){

			if (input_filename_!=NULL){
				input_descriptor_= open(input_filename_, O_RDONLY);
				dup2(input_descriptor_,0);
			}

			if (output_filename_!=NULL){
				output_descriptor_ = open(output_filename_, O_WRONLY | O_CREAT| O_TRUNC, 0666);
				dup2(output_descriptor_,1);
			}

			if (error_filename_!=NULL){
				error_descriptor_ = open(error_filename_, O_WRONLY | O_CREAT| O_TRUNC, 0666);
				dup2(error_descriptor_,2);
			}


			execvp(arguments_[0],arguments_);
			perror("Execute Subprocess Fail!");
			exit(EXIT_FAILURE);
		}
		else {
			int status;
			waitpid(pid, &status, 0);

			if (WIFEXITED(status)) {  
        		std::cout<<"Program exited with status " << WEXITSTATUS(status)<<std::endl;        
    		} 
    		else {
    			if ( WIFSIGNALED(status) ) {     
  					std::cout<<"Program was killed by signal " << WTERMSIG(status)<<std::endl;  
    			}
    		}
    		if (input_descriptor_!=0)
    			close(input_descriptor_);
    		if (output_descriptor_!=1)
    			close(output_descriptor_);
    		if (error_descriptor_!=2)
    			close(error_descriptor_);
		}
	}

}


void Command::ExecuteList(int i,int command_num) {
	if ( CheckSlash((*this)[0]) ) {
		if (FindInDir()) {
			ForkToExecuteList(i,command_num);
			return;
		} else {
			std::cout << "Command " << (*this)[0] << " not found" << std::endl;
		}
	} else {
		if (FindInPath()) {
			ForkToExecuteList(i,command_num);
			return;
		} else {
			std::cout << "Command " << (*this)[0] << " not found" << std::endl;
		}
	}


}


void Command::ForkToExecuteList(int i, int command_num) {



	pid_t pid;
	pid = fork();
	if (pid==-1) {
		perror("Fork Fail!");
		exit(EXIT_FAILURE);
	}
	else {
		if (pid==0){
			/*
			if (input_filename_!=NULL){
				input_descriptor_= open(input_filename_, O_RDONLY);
				dup2(input_descriptor_,0);
			}

			if (output_filename_!=NULL){
				output_descriptor_ = open(output_filename_, O_WRONLY | O_CREAT| O_TRUNC, 0666);
				dup2(output_descriptor_,1);
			}
			*/

			if (error_filename_!=NULL){
				error_descriptor_ = open(error_filename_, O_WRONLY | O_CREAT| O_TRUNC, 0666);
				dup2(error_descriptor_,2);
			}
			if (i == 0)
				setpgid(0, 0);
		
			if (input_descriptor_ != 0)
			{
				close(0);
				dup(input_descriptor_);
			}
			if (input_descriptor_ != 1)
			{
				close(1);
				dup(input_descriptor_);
			}

			int j;
			for (j=3; j<OPEN_MAX; ++j)
				close(j);

			execvp(arguments_[0],arguments_);
			perror("Execute Subprocess Fail!");
			exit(EXIT_FAILURE);
		}
		else {
			if (i==command_num-1) {
				int status;
				waitpid(pid, &status, 0);

				if (WIFEXITED(status)) {  
        			std::cout<<"Program exited with status " << WEXITSTATUS(status)<<std::endl;        
    			} 
    			else {
    				if ( WIFSIGNALED(status) ) {     
  						std::cout<<"Program was killed by signal " << WTERMSIG(status)<<std::endl;  
    				}
    			}
			}
			/*
    		if (input_descriptor_!=0)
    			close(input_descriptor_);
    		if (output_descriptor_!=1)
    			close(output_descriptor_);
    		if (error_descriptor_!=2)
    			close(error_descriptor_);
    		*/
		}
	}

}





