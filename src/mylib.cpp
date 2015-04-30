#include "base64.h"
#include "File_handle.h"
#include "Handle_in_mail.h"
#include "Handle_out_mail.h"
#include "string_handle.h"

extern "C"
{
	int Get_last_number(int i);
	int Is_have_attachment(int f_number,int model);
	int Mail_delete(int f_number,int model);
	int Get_information(int f_number,
			    char* date,
			    char* my_address,
			    char* he_address,
			    char* mail_content,
			    char* subject,int model);

	int Get_attachment_information(int f_number,
				       char* date,
				       char* my_address,
				       char* he_address,
				       char* mail_content,
				       char* subject,
				       char* attachment_subject,int model);
	void Get_attachment(int file_number,char* attachment_way,int model);
   
   int Out_text_write(char* my_email_address,
                   	 char* he_email_address,
                      char* subject,
                   	 char* text_content,
                   	 char* pop_file_name);

	int Out_attachment_write(char* my_email_address,
                         	 char* he_email_address,
                         	 char* subject,
                         	 char* text_content,
                         	 char* out_attachment_file_name,
                         	 char* pop_file_name);



	int Get_last_number(int i)
	{
		return get_last_number(i);
	}
	
	int Is_have_attachment(int f_number,int model)
	{
		return is_have_attachment(f_number,model);
	}
	
	int Mail_delete(int f_number,int model)
	{
		return mail_delete(f_number,model);
	}
	
	int Get_information(int f_number,
			    char* date,
			    char* my_address,
			    char* he_address,
			    char* mail_content,
			    char* subject,int model)
	{
		return get_information(f_number,date,my_address,he_address,mail_content,subject,model);
	}

	
	int Get_attachment_information(int f_number,
				       char* date,
				       char* my_address,
				       char* he_address,
				       char* mail_content,
				       char* subject,
				       char* attachment_subject,int model)
	{
		return get_attachment_information(f_number,date,my_address,he_address,mail_content,subject,attachment_subject,model);
	}
	
	void Get_attachment(int file_number,char* attachment_way,int model)
	{
		get_attachment(file_number,attachment_way,model);
	}
	
	int Out_text_write(char* my_email_address,
                   	 char* he_email_address,
                      char* subject,
                   	 char* text_content,
                   	 char* pop_file_name)
       {
          	return out_text_write(my_email_address,he_email_address,subject,text_content,pop_file_name);
       }
       
   int Out_attachment_write(char* my_email_address,
                         	 char* he_email_address,
                         	 char* subject,
                         	 char* text_content,
                         	 char* out_attachment_file_name,
                         	 char* pop_file_name)
        {
        	return out_attachment_write(my_email_address,
           									 he_email_address,
                         	 			 subject,
                         	 			 text_content,
                         	 			 out_attachment_file_name,
                         	 			 pop_file_name);  
         }
         
} 
