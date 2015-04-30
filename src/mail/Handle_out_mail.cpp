#include<ctime>
#include<fstream>
#include<iostream>
#include"Handle_out_mail.h"
#include"base64.h"
#include"string_handle.h"
#include"File_handle.h"

int out_text_write(std::string my_email_address,
	       	   std::string he_email_address,
	           std::string subject,
	           char* text_content,
	           std::string pop_file_name){
	std::ofstream out(pop_file_name.c_str(),std::ios::out);
	
	std::ifstream local_in(pop_file_name.c_str(),std::ios::in|std::ios::binary);
	std::ofstream local_out("out.txt",std::ios::out|std::ios::app);
	
	std::string s="ff\n\nk";
	char* _buf;
	int sendsize;
	int _sendsize;
	int my_ops=my_email_address.find('@',0);
	std::string my_header_mail=my_email_address.substr(0,my_ops);
	std::string my_dns_mail=my_email_address.substr(my_ops+1,my_email_address.size()-my_ops);
	
	int he_ops=he_email_address.find('@',0);
	std::string he_header_mail=he_email_address.substr(0,he_ops);
	std::string he_dns_mail=he_email_address.substr(he_ops+1,he_email_address.size()-he_ops);
	
	time_t my_time;
	my_time=time(NULL);
	std::string str_time=ctime(&my_time);
	std::string str_week=string_date_get_week(str_time);
	std::string str_month=string_date_get_month(str_time);
	std::string str_date=string_date_get_date(str_time);
	std::string str_only_time=string_date_get_time(str_time);
	std::string str_year=string_date_get_year(str_time);
	
	out<<"Date: "<<str_week<<", "<<str_date<<" "<<str_month;
	out<<" "<<str_year<<" "<<str_only_time<<" +0800"<<std::endl;
	
	out<<"From: \""<<my_header_mail<<"\" <"<<my_email_address<<">"<<std::endl;
	out<<"To: \""<<he_header_mail<<"\" <"<<he_email_address<<">"<<std::endl;
	
	out<<"Subject: "<<subject<<std::endl;
	
	out<<"Message-ID: <"<<str_year<<str_month<<str_date<<string_empty_trim(str_only_time)
			<<"@"<<my_dns_mail<<">"<<std::endl;
	out<<"X-mailer: Foxmail 6, 10, 201, 20 [cn]"<<std::endl;
	out<<"Mime-Version: 1.0"<<std::endl;
	out<<"Content-Type: multipart/alternative;"<<std::endl;
	std::string boundary="=====003_ZB"+string_empty_trim(str_time)+"_=====";
	out<<"        boundary=\""+boundary+"\""<<std::endl;   //note random
	out<<std::endl;
	out<<"This is a multi-part message in MIME format."<<std::endl;
	out<<std::endl;
	out<<"--"<<boundary<<std::endl;
	out<<"Content-Type: text/plain;"<<std::endl;
	out<<"        charset=\"gb2312\""<<std::endl;
	out<<"Content-Transfer-Encoding: base64"<<std::endl;
	out<<std::endl;
	std::string test_str=text_content;
	_buf=base64_encode(text_content,strlen(text_content));
	_sendsize=strlen(_buf);
	out.write(_buf,static_cast<std::streamsize>(_sendsize));
	out<<std::endl;
	out<<std::endl;
	out<<"--"<<boundary<<std::endl;
	out<<std::endl;
	int num=get_last_number(0);
	char buf[1025];
	memset(buf,'\0',1025);
	local_out<<std::endl;
	while(local_in.read(buf,1024)){
		local_out.write(buf,1024);
	}
	local_out.write(buf,local_in.gcount());
	local_out<<std::endl;
	char file_number[1024];
	sprintf(file_number,"%d",num+1);
	local_out<<"!@#$%"<<file_number<<"!@#$%"<<std::endl;
	return 1;
} 

int out_attachment_write(std::string my_email_address,
			 std::string he_email_address,
			 std::string subject,
			 char* text_content,
			 std::string out_attachment_file_name,
			 std::string pop_file_name){
	
	std::ifstream in(out_attachment_file_name.c_str(),std::ios::in|std::ios::binary);
	std::ofstream out(pop_file_name.c_str(),std::ios::out);
	
	std::ifstream local_in(pop_file_name.c_str(),std::ios::in|std::ios::binary);
	std::ofstream local_out("out.txt",std::ios::out|std::ios::app);
	
	std::string s="ff\n\nk";
	char* _buf;
	int sendsize;
	int _sendsize;
	int my_ops=my_email_address.find('@',0);
	std::string my_header_mail=my_email_address.substr(0,my_ops);
	std::string my_dns_mail=my_email_address.substr(my_ops+1,my_email_address.size()-my_ops);
	
	int he_ops=he_email_address.find('@',0);
	std::string he_header_mail=he_email_address.substr(0,he_ops);
	std::string he_dns_mail=he_email_address.substr(he_ops+1,he_email_address.size()-he_ops);
	
	time_t my_time;
	my_time=time(NULL);
	std::string str_time=ctime(&my_time);
	std::string str_week=string_date_get_week(str_time);
	std::string str_month=string_date_get_month(str_time);
	std::string str_date=string_date_get_date(str_time);
	std::string str_only_time=string_date_get_time(str_time);
	std::string str_year=string_date_get_year(str_time);
	
	out<<"Date: "<<str_week<<", "<<str_date<<" "<<str_month;
	out<<" "<<str_year<<" "<<str_only_time<<" +0800"<<std::endl;
	
	out<<"From: \""<<my_header_mail<<"\" <"<<my_email_address<<">"<<std::endl;
	out<<"To: \""<<he_header_mail<<"\" <"<<he_email_address<<">"<<std::endl;
	
	out<<"Subject: "<<subject<<std::endl;
	
	out<<"Message-ID: <"<<str_year<<str_month<<str_date<<string_empty_trim(str_only_time)
			<<"@"<<my_dns_mail<<">"<<std::endl;
	out<<"X-mailer: Foxmail 6, 10, 201, 20 [cn]"<<std::endl;
	out<<"Mime-Version: 1.0"<<std::endl;
	out<<"Content-Type: multipart/mixed;"<<std::endl;
	std::string boundary="=====001_ENNAN"+string_empty_trim(str_time)+"_=====";
	out<<"        boundary=\""+boundary+"\""<<std::endl;   //note random
	out<<std::endl;
	out<<"This is a multi-part message in MIME format."<<std::endl;
	out<<std::endl;
	out<<"--"<<boundary<<std::endl;
	
	std::string text_boundary="=====003_ZB"+string_empty_trim(str_time)+"_=====";
	out<<"Content-Type: multipart/alternative;"<<std::endl;
	out<<"        boundary=\""+text_boundary+"\""<<std::endl;   //note random
	
	out<<std::endl;
	out<<std::endl;
	
	out<<"--"<<text_boundary<<std::endl;
	out<<"Content-Type: text/plain;"<<std::endl;
	out<<"        charset=\"gb2312\""<<std::endl;
	out<<"Content-Transfer-Encoding: base64"<<std::endl;
	out<<std::endl;
	std::string test_str=text_content;
	_buf=base64_encode(text_content,test_str.size());
	_sendsize=strlen(_buf);
	out.write(_buf,static_cast<std::streamsize>(_sendsize));
	out<<std::endl;
	out<<std::endl;
	out<<"--"<<text_boundary<<std::endl;
	out<<"--"<<boundary<<std::endl;
	//local_out<<std::endl;
	std::string attachment_name=string_file_name(out_attachment_file_name);
	char* x_buf=base64_encode(str_to_char(attachment_name),strlen(str_to_char(attachment_name)));
	
	out<<"Content-Type: application/app"<<std::endl;
	out<<"	      name=\"=?gb2312?B?"<<x_buf<<"?=\""<<std::endl;
	out<<"Content-Transfer-Encoding: base64"<<std::endl;
	out<<"Content-Disposition: attachment;"<<std::endl;
	//std::string attachment_name=string_file_name(out_attachment_file_name);
	//char* x_buf=base64_encode(str_to_char(attachment_name),strlen(str_to_char(attachment_name)));
	out<<"        filename=\"=?gb2312?B?"<<x_buf<<"?=\""<<std::endl;
	out<<std::endl;
	char buf[1008+1];
	memset(buf,'\0',1008+1);
	while(1){
		in.read(buf,static_cast<std::streamsize>(1008));
		sendsize=in.gcount();
		_buf=base64_encode(buf,sendsize);
		_sendsize=strlen(_buf);
		if(sendsize<0){
			std::cout<<"error"<<std::endl;
		}
		else if(sendsize==0)
			break;
		else{
			out.write(_buf,static_cast<std::streamsize>(_sendsize));
			//cout<<_sendsize<<endl;
		}
	}
	
	out<<std::endl;
	out<<"--"<<boundary<<std::endl;
	out<<std::endl;
	int num=get_last_number(0);
	char read_buf[1025];
	memset(read_buf,'\0',1025);
	out<<std::endl;
	while(local_in.read(read_buf,1024)){
		local_out.write(read_buf,1024);
	}
	local_out.write(read_buf,local_in.gcount());
	local_out<<std::endl;
	char file_number[1024];
	sprintf(file_number,"%d",num+1);
	local_out<<"!@#$%"<<file_number<<"!@#$%"<<std::endl;
	return 1;
} 
