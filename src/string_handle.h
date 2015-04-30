#ifndef STRING_HANDLE_H
#define STRING_HANDLE_H

#include<string>
#include<fstream>

std::string string_empty_trim(std::string s);
std::string string_date_get_week(std::string str_date);
std::string string_date_get_month(std::string str_date);
std::string string_date_get_date(std::string str_date);
std::string string_date_get_time(std::string str_date);
std::string string_date_get_year(std::string str_date);
std::string string_file_name(std::string str_file);
char* str_to_char(std::string str);
char* file_gets(char* s,int _n,std::ifstream& iop);
char* get_lines(char* s,int _n,std::ifstream& iop);
void filter_n(char* buf);

#endif
