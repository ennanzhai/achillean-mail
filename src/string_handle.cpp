#include"string_handle.h"
#include<string>
#include<cstring>
#include<iostream>
#include<fstream>

std::string string_empty_trim(std::string s){
	int j=0;
	std::string final="";
	int first=0;
	for(int last=0;last!=s.size();++last){
		if(s[last]==' '||s[last]==':'){
			final+=s.substr(first,last-first);
			first=last+1;
		}
	}
	return final;
}

std::string string_date_get_week(std::string str_date){
	return str_date.substr(0,3);
}

char* str_to_char(std::string str){
	char* buf=new char[str.size()+1];
	strcpy(buf,str.c_str());
	buf[str.size()]='\0';
	return buf;
}

std::string string_date_get_month(std::string str_date){
	return str_date.substr(4,3);
}

std::string string_date_get_date(std::string str_date){
	int last=2;
	if(str_date[9]==' ')
		last=1;
	return str_date.substr(8,last);
}

std::string string_date_get_time(std::string str_date){
	int j=0;
	for(j=str_date.size()-1;str_date[j]!=' ';--j)
		;
	return str_date.substr(j-8,8);
}

std::string string_date_get_year(std::string str_date){
	return str_date.substr(str_date.size()-1-4,4);
}

std::string string_file_name(std::string str_file){
	int j=0;
	int count=0;
	for(j=str_file.size()-1;str_file[j]!='/';--j)
		++count;
	return str_file.substr(j+1,count);
}

char* file_gets(char* s,int _n,std::ifstream& iop){
	register int c;
	register char* cs;
	int n=_n+1;
	cs=s;
	int key=0;
	
	while((--n>0)&&((c=iop.get())!=EOF)){
		if(!isspace(c)){
			if(c=='-'){
				*cs='\0';
				return NULL;
			}
			*cs++=c;
		}
		else{
			++n;
		}
	}
	*cs='\0';
	return (c==EOF)?NULL:cs;
}

char* get_lines(char* s,int _n,std::ifstream& iop){
	register int c;
	register char* cs;
	int n=_n+1;
	cs=s;
	while((--n>0)&&((c=iop.get())!=EOF)){
		if((*cs++=c)=='\n')
			break;
	}
	*cs='\0';
	
	return ((c==EOF)&&(cs==s))?NULL:cs;
}

void filter_n(char* buf){
	char* temp=new char[strlen(buf)];
	strcpy(temp,buf);
	char* final=buf;
	for(int i=0;i!=strlen(temp);++i){
		if(!isspace(temp[i]))
			*final++=temp[i];
	}
	final='\0';
}





















