#ifndef BASE64_H
#define BASE64_H

#include<string>
std::string base64_decode(std::string const& s);
char* base64_encode(char* bytes_to_encode,int in_len);

#endif
