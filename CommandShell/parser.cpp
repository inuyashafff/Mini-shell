//Copyright:
//License:
//Author:
//Time:
//Description:

#include "parser.h"
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include "command.h"

Parser::Parser() {

}

int Parser::ReadLine() {
	if (!getline(std::cin, command_line_))
		return 0;
	else return 1;

}

bool Parser::isEmptyLine() {
	if (command_line_.empty())
		return true;
	std::string::iterator it = command_line_.begin();
	SkipSpace(it);
	if (it==command_line_.end())
		return true;
	else return false;   //需要改，如果跳过所有空格和制表符，到末尾了，也是空的
}

void Parser::SkipSpace(std::string::iterator &it) {
	while ((*it == ' ' || *it == '\t') && it!=command_line_.end())
		it++;
}

std::string Parser::GetVarName(std::string str, std::string::iterator it) {
	std::string ans;
	str.push_back(' ');

	while (it!=str.end()) {
               if ((*it>='a' &&  *it<='z') ||
                   (*it>='A' &&  *it<='Z') ||
                   (*it>='0' &&  *it<='9') ||
                   (*it=='_')) {
			ans.push_back(*it);
			it++;
			continue;			

                } else {
			return ans;
                }

	}
	return ans;

}


std::string Parser::VariableSubstitution(std::string str, const std::map<std::string,std::string> var_table) {
        std::string ans;
	
	std::string::iterator it=str.begin();
	bool slash=false;
	while (it!=str.end()) {
		if (slash==true){
			ans.push_back(*it);
			slash=false;
			it++;
			continue;
		} else {
			if (*it == '$') {
				std::string varname=GetVarName(str,it+1);
				std::map<std::string,std::string>::const_iterator found;
                		found = var_table.find(varname);
                		if (found == var_table.end()) {
                                        it=it+varname.length()+1;
                                        continue;
                		} else {
					ans+=found->second;
					it=it+varname.length()+1;
					continue;
				}
                        } else if (*it == '\\'){
				ans.push_back(*it);
				slash=true;
				it++;
				continue;
			} else {
				ans.push_back(*it);
                                it++;
                                continue;
			}
		
		}

	}
	return ans;

}



void Parser::Parse(std::vector<char*> &result, const std::map<std::string,std::string> var_table, std::string sub_command_line) {


	// 处理 $ 进行替换
	sub_command_line=VariableSubstitution(sub_command_line,var_table);

	//开始处理‘ ’和tab和转义字符

	sub_command_line.push_back(' ');
	std::string::iterator it=sub_command_line.begin();
	SkipSpace(it);

	std::string current_arg;
	char* cstr_ptr = NULL;

	//处理set进行特殊处理，
	bool is_set = false;
	while (it!= sub_command_line.end()) {
		if ((*it == ' '|| *it == '\t')&&(is_set==false || it==sub_command_line.end()-1)) {
			cstr_ptr = new char [current_arg.length()+1];
			std::strcpy (cstr_ptr, current_arg.c_str());
			result.push_back(cstr_ptr);
			if ((std::strcmp(result[0],"set")==0) && result.size()==2)
				is_set=true; 
			current_arg.clear();
			SkipSpace(it);
			continue;
		} else if (*it == '\\') {
			if ((*(it+1) == ' ' || *(it+1) == '\t' || *(it+1) == '\\') && it+1!=sub_command_line.end()-1 ) {
				current_arg.push_back(*(it+1));
				it=it+2;
			}
			else {
				it++;
			}
			continue;

		} else {
			current_arg.push_back(*it);
			it++;
			continue;
		}
		
	}



	result.push_back(NULL);

}


int Parser::ParseLine(std::vector<std::vector<char*> > &result, const std::map<std::string,std::string> var_table) {
	std::vector<std::string> str_array;
	int lastpipe=-1;
	bool isempty = true;
	int i;
	for (i=0; i<command_line_.length(); i++) {
		if (command_line_[i] == '|') {
			if (isempty) {
				return 0;
			} else {
				str_array.push_back(command_line_.substr(lastpipe+1,i-lastpipe-1));
				lastpipe=i;
				isempty=true;

			}

		} else if ((command_line_[i] != ' ') && (command_line_[i] != '\t')) {
			isempty=false;
		}

	}

	if (isempty) {
		return 0;
	} else {
		str_array.push_back(command_line_.substr(lastpipe+1,i-lastpipe-1));
		lastpipe=i;
		isempty=true;
	}


	for (i=0; i<str_array.size(); i++) {
		std::vector<char*> ans;
		Parse(ans,var_table,str_array[i]);
		result.push_back(ans);
		ans.clear();
	}

	return 1;

	
}










