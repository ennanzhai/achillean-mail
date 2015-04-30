#include<string>
#include<iostream>
#include<cstring>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include"string_handle.h"
#include"Handle_in_mail.h"
#include"base64.h"

using namespace std;

char* get_date(char* pop_file_name){
	ifstream in(pop_file_name,ios::binary|ios::in);
	char buf[1024];
	string s_read;
	int area;
	int back;
	char s[1024];
	char* cs=s;
	char* final=cs;
	int i=0;
	while(get_lines(buf,1024,in)){
		s_read=buf;
		
		area=s_read.find("Date:",0);
		if(area>=0&&area<=10){
			for(i=area+5;;++i){
				if(buf[i]=='+'){
					back=i-1;
					break;
				}
			}
			for(i=area+6;i<=back;++i){
				*cs++=buf[i];
			}
			*cs='\0';
			return final;	
		}	
	}
	return NULL;
}

char* get_he_address(char* pop_file_name){
	ifstream in(pop_file_name,ios::binary|ios::in);
	char buf[1024];
	string s_read;
	int area;
	int back;
	char s[1024];
	char* cs=s;
	char* final=cs;
	int i=0;
	while(get_lines(buf,1024,in)){
		s_read=buf;
		area=s_read.find("From:",0);
		if(area>=0&&area<=10){
			for(i=area+5;i<=strlen(buf)-1;++i){
				if(buf[i]=='<')
					area=i+1;
				if(buf[i]=='>')
					back=i-1;
			}
			for(i=area;i<=back;++i){
				*cs++=buf[i];
			}
			*cs='\0';
			return final;	
		}	
	}
	return NULL;
}

char* get_my_address(char* pop_file_name){
	ifstream in(pop_file_name,ios::binary|ios::in);
	char buf[1024];
	string s_read;
	int area;
	int back;
	char s[1024];
	char* cs=s;
	char* final=cs;
	int i=0;
	while(get_lines(buf,1024,in)){
		s_read=buf;
		area=s_read.find("To:",0);
		if(area>=0&&area<=10){
			for(i=area+3;i<=strlen(buf)-1;++i){
				if(buf[i]=='<')
					area=i+1;
				if(buf[i]=='>')
					back=i-1;
			}
			for(i=area;i<=back;++i){
				*cs++=buf[i];
			}
			*cs='\0';
			return final;	
		}	
	}
	return NULL;
}

char* get_subject(char* pop_file_name){
	ifstream in(pop_file_name,ios::binary|ios::in);
	char buf[1024];
	string s_read;
	int area;
	int back;
	char s[1024];
	char* cs=s;
	char* final=cs;
	int i=0;
	while(get_lines(buf,1024,in)){
		s_read=buf;
		area=s_read.find("Subject:",0);
		if(area>=0&&area<=10){
			for(i=area+9;i<=strlen(buf)-2;++i){
				*cs++=buf[i];
			}
			*cs='\0';
			return final;	
		}	
	}
	return NULL;
}

int get_information(int f_number,
		     char* date,
		     char* my_address,
		     char* he_address,
		     char* mail_content,
		     char* subject,
		     int model){
	char file_number[1024];
	sprintf(file_number,"%d",f_number);
	char buf[1024];
	string str_buf,temp;
	string file_name;
	if(model==0){
		file_name="out_delete.txt";
	}else{
		file_name="in_delete.txt";
	}
	ifstream in_d(file_name.c_str());
	while(get_lines(buf,888,in_d)){
		temp=buf;
		if(strncmp(buf,file_number,strlen(file_number))==0)
			return 0;
	}
	in_d.close();
	if(model==0){
		file_name="out.txt";
	}else{
		file_name="in.txt";
	}
	ifstream in(file_name.c_str());
	int key=0;
	int i,area,back,find_area;
	char* return_date=date;
	char* return_my_address=my_address;
	char* return_he_address=he_address;
	char* return_subject=subject;
	char* return_content=mail_content;
	string boundary;
	char array_boundary[1024];
	int need_decode=0;
	int count=0;
	int line_count=0;
	char decode_buf[889];
	string decoded;
	while(get_lines(buf,888,in)){
		if(count==5){
			break;
		}
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
		}else if(key==1){
			area=str_buf.find("Content-Type: text/plain",0);
			if(area>=0&&area<=10){
				key=2;
			}
			area=str_buf.find("Subject:",0);
			if(area>=0&&area<=30){
				int back1=str_buf.rfind("?=",str_buf.size()-1);
				int back2=str_buf.rfind("B?",str_buf.size()-1);
				if(back1==-1&&back2==-1){
					strcat(subject,str_to_char(str_buf.substr(9,str_buf.size()-1-9)));
				}else{
					back2=back2+2;
					strcat(subject,str_to_char(base64_decode(str_buf.substr(back2,back1-back2))));
				}
				++count;
				continue;
			}
			area=str_buf.find("To:",0);
			if(area>=0&&area<=5){
				int _find=str_buf.rfind("<",str_buf.size()-1);
				if(_find>=0){
					for(i=area+3;i<=strlen(buf)-1;++i){
						if(buf[i]=='<')
							area=i+1;
						if(buf[i]=='>')
							back=i-1;
					}
					for(i=area;i<=back;++i){
						*return_my_address++=buf[i];
					}
					*return_my_address='\0';
				}else{
					for(i=area+4;i<=strlen(buf)-2;++i){
						*return_my_address++=buf[i];
					}
				
					*return_my_address='\0';
				}
				++count;
				continue;
			}
			area=str_buf.find("From:",0);
			if(area>=0&&area<=5){
				for(i=area+5;i<=strlen(buf)-1;++i){
					if(buf[i]=='<')
						area=i+1;
					if(buf[i]=='>')
						back=i-1;
				}
				for(i=area;i<=back;++i){
					*return_he_address++=buf[i];
				}
				*return_he_address='\0';
				++count;
				continue;
			}		
			area=str_buf.find("Date:",0);
			if(area>=0&&area<=10){
				for(i=area+5;;++i){
					if(buf[i]=='+'){
						back=i-1;
						break;
					}
				}
				for(i=area+6;i<=back;++i){
					*return_date++=buf[i];
				}
				*return_date='\0';
				++count;
				continue;
			}	
			
			area=str_buf.find("Content-Type: multipart/alternative",0);
			if(area>=0&&area<=10){
				key=3;
				continue;
			}
			
		}else if(key==2){
			str_buf=buf;
			if(str_buf.find(boundary,0)<=100&&str_buf.find(boundary,0)>=0){
				key=1;
				++count;
				if(need_decode==1){
					str_buf=return_content;
					return_content[0]='\0';
					strcat(return_content,str_to_char((base64_decode(str_buf))));
				}
				continue;
			}	
			find_area=str_buf.find("Content-Transfer-Encoding: base64",0);
			if(find_area<=10&&find_area>=0){
				need_decode=1;
				continue;
			}
			find_area=str_buf.find("Content-Transfer-Encoding: 7bit",0);
			if(find_area<=10&&find_area>=0){
				need_decode=2;
				line_count=1;
				continue;
			}
			if(need_decode==2&&line_count==1){
				line_count++;
				continue;
			}
			if(need_decode==2&&line_count==2){
				strcat(return_content,buf);
				continue;
			}
			if(str_buf.find("Content-Transfer-Encoding: base64",0)>=30
					&&str_buf.find("Content-Disposition: inline",0)>=30
					&&!isspace(buf[0])
			  		&&need_decode==1){
				buf[strlen(buf)-1]='\0';
				strcat(return_content,buf);
			}
		}else{
			str_buf=buf;
			area=str_buf.find("boundary=",0);
			if(area>=0&&area<=100){
				int back1=str_buf.find("\"",0);
				int back2=str_buf.rfind("\"",str_buf.size()-1);
				back1=back1+1;
				strcat(array_boundary,str_to_char(str_buf.substr(back1,back2-back1)));
				boundary=array_boundary;
				key=1;
			}
		}	
	}
	in.close();
	return 0;
}

int get_attachment_information(int f_number,
				char* date,
				char* my_address,
				char* he_address,
				char* mail_content,
				char* subject,
				char* attachment_subject,
			       	int model){
	char file_number[1024];
	sprintf(file_number,"%d",f_number);
	char buf[1024];
	string str_buf,temp;
	string file_name;
	if(model==0){
		file_name="out_delete.txt";
	}else{
		file_name="in_delete.txt";
	}
	ifstream in_d(file_name.c_str());
	while(get_lines(buf,888,in_d)){
		temp=buf;
		
		if(strncmp(buf,file_number,strlen(file_number))==0)
			return 0;
	}
	in_d.close();
	if(model==0){
		file_name="out.txt";
	}else{
		file_name="in.txt";
	}
	ifstream in(file_name.c_str());
	int key=0;
	int i,area,back,find_area;
	char* return_date=date;
	char* return_my_address=my_address;
	char* return_he_address=he_address;
	char* return_subject=subject;
	char* return_content=mail_content;
	char* return_attachment_subject=attachment_subject;
	string boundary;
	char array_boundary[1024];
	int need_decode=0;
	int count=0;
	int line_count=0;
	char decode_buf[889];
	string decoded;
	while(get_lines(buf,888,in)){
		if(count==6){
			break;
		}
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
		}else if(key==1){
			area=str_buf.find("Content-Type: text/plain",0);
			if(area>=0&&area<=10){
				key=2;
				continue;
			}
			area=str_buf.find("Subject:",0);
			if(area>=0&&area<=30){
				int back1=str_buf.rfind("?=",str_buf.size()-1);
				int back2=str_buf.rfind("B?",str_buf.size()-1);
				if(back1==-1&&back2==-1){
					strcat(subject,str_to_char(str_buf.substr(9,str_buf.size()-1-9)));
				}else{
					back2=back2+2;
					strcat(subject,str_to_char(base64_decode(str_buf.substr(back2,back1-back2))));
				}
				++count;
				continue;
			}
			area=str_buf.find("filename=",0);
			if(area>=0&&area<=100){
				int back1=str_buf.rfind("?=",str_buf.size()-1);
				int back2=str_buf.rfind("B?",str_buf.size()-1);
				back2=back2+2;
				strcat(attachment_subject,str_to_char(base64_decode(str_buf.substr(back2,back1-back2))));
				++count;
				continue;
			}	
			area=str_buf.find("To:",0);
			if(area>=0&&area<=5){
				int _find=str_buf.rfind("<",str_buf.size()-1);
				if(_find>=0){
					for(i=area+3;i<=strlen(buf)-1;++i){
						if(buf[i]=='<')
							area=i+1;
						if(buf[i]=='>')
							back=i-1;
					}
					for(i=area;i<=back;++i){
						*return_my_address++=buf[i];
					}
					*return_my_address='\0';
				}else{
					for(i=area+4;i<=strlen(buf)-2;++i){
						*return_my_address++=buf[i];
					}
				
					*return_my_address='\0';
				}
				++count;
				continue;
			}
			area=str_buf.find("From:",0);
			if(area>=0&&area<=5){
				for(i=area+5;i<=strlen(buf)-1;++i){
					if(buf[i]=='<')
						area=i+1;
					if(buf[i]=='>')
						back=i-1;
				}
				for(i=area;i<=back;++i){
					*return_he_address++=buf[i];
				}
				*return_he_address='\0';
				++count;
				continue;
			}		
			area=str_buf.find("Date:",0);
			if(area>=0&&area<=10){
				for(i=area+5;;++i){
					if(buf[i]=='+'){
						back=i-1;
						break;
					}
				}
				for(i=area+6;i<=back;++i){
					*return_date++=buf[i];
				}
				*return_date='\0';
				++count;
				continue;
			}	
			area=str_buf.find("Content-Type: multipart/alternative",0);
			if(area>=0&&area<=10){
				key=3;
				continue;
			}
			
			
		}else if(key==2){
			str_buf=buf;
			if(str_buf.find(boundary,0)<=100&&str_buf.find(boundary,0)>=0){
				key=1;
				++count;
				if(need_decode==1){
					str_buf=return_content;
					return_content[0]='\0';
					strcat(return_content,str_to_char((base64_decode(str_buf))));
				}
				continue;
			}	
			find_area=str_buf.find("Content-Transfer-Encoding: base64",0);
			if(find_area<=10&&find_area>=0){
				need_decode=1;
				continue;
			}
			find_area=str_buf.find("Content-Transfer-Encoding: 7bit",0);
			if(find_area<=10&&find_area>=0){
				need_decode=2;
				line_count=1;
				continue;
			}
			if(need_decode==2&&line_count==1){
				line_count++;
				continue;
			}
			if(need_decode==2&&line_count==2){
				strcat(return_content,buf);
				continue;
			}
			if(str_buf.find("Content-Transfer-Encoding: base64",0)>=30
					&&str_buf.find("Content-Disposition: inline",0)>=30
					&&!isspace(buf[0])
			  		&&need_decode==1){
				buf[strlen(buf)-1]='\0';
				strcat(return_content,buf);
			}
		}else{
			str_buf=buf;
			area=str_buf.find("boundary=",0);
			if(area>=0&&area<=100){
				int back1=str_buf.find("\"",0);
				int back2=str_buf.rfind("\"",str_buf.size()-1);
				back1=back1+1;
				strcat(array_boundary,str_to_char(str_buf.substr(back1,back2-back1)));
				boundary=array_boundary;
				key=1;
			}	
		}
	}
	in.close();
	return 1;
}

///////////////////////////////////////获得附件///////////////////////////////////////////
///////////
///////////////////////////////////////////////////////////////////////////////////////
char* get_attachment_subject(char* filename){
	string str_filename=filename;
	int back1=str_filename.rfind("?=",str_filename.size()-1);
	int back2=str_filename.rfind("B?",str_filename.size()-1);
	back2=back2+2;
	return str_to_char(base64_decode(str_filename.substr(back2,back1-back2)));
}

int get_attachment_from_num(char* file_number,char* attachment_way,int model);
void get_attachment(int file_number,char* attachment_way,int model){
	char str_file_number[1024];
	sprintf(str_file_number,"%d",file_number);
	get_attachment_from_num(str_file_number,attachment_way,model);
}

int get_attachment_from_num(char* file_number,char* attachment_way,int model){
	string str_number;
	string file_num=file_number;
	string file_name;
	if(model==0){
		file_name="out.txt";
	}else{
		file_name="in.txt";
	}
	ifstream in(file_name.c_str());
	
	char array_attachment_subject[1024];
	char* attachment_subject= array_attachment_subject;
	char buf[1024];
	int area;
	int back;
	int i=0;
	string temp;
	int key=0;
	while(get_lines(buf,1024,in)){
		str_number=buf;
		switch(key){
			case 0:{
				area=str_number.find("!@#$%",0);
				back=str_number.rfind("!@#$%",str_number.size()-1);
			
				if((area>=0&&area<=10)&&(back>=6&&back<=17)){
					temp=str_number.substr(area+5,back-area-5);
					if(file_number==temp){
						key=1;
					}
				}
				break;
			}
			case 1:{
				area=str_number.find("Content-Disposition: attachment;",0);
				if(area>=0&&area<=10){
					key=2;
				}
				area=str_number.find("filename=",0);
				if(area>=0&&area<=1024){
					key=3;
					attachment_subject=get_attachment_subject(buf);
					
				}
				break;
			}
			case 2:{
				area=str_number.find("filename=",0);
				if(area>=0&&area<=50){
					key=3;
					attachment_subject=get_attachment_subject(buf);
				}
				break;
			}
			default:{
				
			}
			
		}
		if(key==3){
			break;
		}
	}
	char array_attachment_way[1024];
	char* attachment_file_name=array_attachment_way;
	int attachment_way_size=strlen(attachment_way);
	for(i=0;i!=attachment_way_size;++i)
		array_attachment_way[i]=*attachment_way++;
	
	attachment_file_name=strcat(array_attachment_way,attachment_subject);
	ofstream out(attachment_file_name,ios::out|ios::binary);
	char decode_buf[9];
	memset(decode_buf,'\0',9);
	string s;
	string decoded;
	int count=0;
	while(file_gets(decode_buf,8,in)){
		s=decode_buf;
		decoded=base64_decode(s);
		out.write(decoded.c_str(),decoded.size());
	}
	s=decode_buf;
	decoded=base64_decode(s);
	out.write(decoded.c_str(),decoded.size());
	cout<<"finished!"<<endl;
	return 1;
}

