#include"File_handle.h"
#include"string_handle.h"
#include<cstdio>
#include<cstdlib>
#include<fstream>

int get_last_number(int model){
	FILE* out;
	if(model==0)
		out=popen("tail -1 out.txt","r");
	else
		out=popen("tail -1 in.txt","r");
	char buf[1024];
	fgets(buf,sizeof(buf),out);
	std::string temp;
	std::string str_buf=buf;
	int area,back;
	area=str_buf.find("!@#$%",0);
	back=str_buf.rfind("!@#$%",str_buf.size()-1);
	
	int i=0;
	if((area>=0&&area<=10)&&(back>=6&&back<=17)){
		temp=str_buf.substr(area+5,back-area-5);
		sscanf(temp.c_str(),"%d",&i); 
	}
	return i;
} 

int is_have_attachment(int f_number,int model){
	char file_number[1024];
	sprintf(file_number,"%d",f_number);
	char buf[1024];
	std::string file_name;
	if(model==0){
		file_name="out.txt";
	}else{
		file_name="in.txt";
	}
	
	std::ifstream in(file_name.c_str());
	buf[1024];
	std::string str_buf,temp;
	int key=0;
	int area,back;
	while(get_lines(buf,888,in)){
		str_buf=buf;
		if(key==0){
			area=str_buf.find("!@#$%",0);
			back=str_buf.rfind("!@#$%",str_buf.size()-1);
			if((area>=0&&area<=10)&&(back>=6&&back<=17)){
				temp=str_buf.substr(area+5,back-area-5);
				if(file_number==temp){
					key=1;
					continue;
				}
			}
		}else{
			area=str_buf.find("Content-Type: multipart/mixed",0);
			if(area>=0&&area<=100){
				return 1;
			}
			area=str_buf.find("Content-Type: text/plain",0);
			if(area>=0&&area<=100){
				break;
			}
		}
	}
	return 0;
}

int mail_delete(int f_number,int model){
	char file_number[1024];
	sprintf(file_number,"%d",f_number);
	char buf[1024];
	std::string file_name;
	if(model==0){
		file_name="out_delete.txt";
	}else{
		file_name="in_delete.txt";
	}
	
	std::ofstream out(file_name.c_str(),std::ios::app|std::ios::out);
	out<<file_number<<std::endl;
	return 1;
}
