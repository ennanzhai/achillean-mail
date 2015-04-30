#ifndef HANDLE_OUT_MAIL_H
#define HANDLE_OUT_MAIL_H

#include<string>

int out_text_write(std::string my_email_address,
	       	   std::string he_email_address,
	           std::string subject,
	           char* text_content,
	           std::string pop_file_name);

int out_attachment_write(std::string my_email_address,
			 std::string he_email_address,
			 std::string subject,
			 char* text_content,
			 std::string out_attachment_file_name,
			 std::string pop_file_name);

#endif 
