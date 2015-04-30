#ifndef HANDLE_IN_MAIL_H
#define HANDLE_IN_MAIL_H

#include<cstdio>

char* get_date(char* pop_file_name);

char* get_he_address(char* pop_file_name);

char* get_my_address(char* pop_file_name);

char* get_subject(char* pop_file_name);

int get_information(int f_number,
		     char* date,
		     char* my_address,
		     char* he_address,
		     char* mail_content,
		     char* subject,int model);
		     
int get_attachment_information(int f_number,
				char* date,
				char* my_address,
				char* he_address,
				char* mail_content,
				char* subject,
				char* attachment_subject,int model);

char* get_attachment_subject(char* filename);

void get_attachment(int file_number,char* attachment_way,int model);

int get_attachment_from_num(char* file_number,char* attachment_way,int model);

#endif
 
