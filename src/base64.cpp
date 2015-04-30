#include"base64.h"
#include<string>
#include<iostream>
#include<cstdio>

static const std::string base64_chars=
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";


static inline bool is_base64(unsigned char c){
	return (isalnum(c)||(c=='+')||(c=='/'));
}

char* base64_encode(char* bytes_to_encode,int in_len){	
	char* buf=new char[in_len*2];
	memset(buf,'\0',in_len*2);
	char* cs=buf;
	int i=0;
	int j=0;
	char char_array_3[3];
	char char_array_4[4];
	int is_handled_13=0;
	while(in_len--){
		char_array_3[i++]=*(bytes_to_encode++);
		
		if(i==3){
			char_array_4[0]=(char_array_3[0]&0xfc)>>2;
			char_array_4[1]=((char_array_3[0]&0x03)<<4)+((char_array_3[1]&0xf0)>>4);
			char_array_4[2]=((char_array_3[1]&0x0f)<<2)+((char_array_3[2]&0xc0)>>6);
			char_array_4[3]=char_array_3[2]&0x3f;

			for(i=0;i<4;++i)
				*buf++=base64_chars[char_array_4[i]];
			i=0;
		}
	}

	if(i){
		for(j=i;j<3;++j)
			char_array_3[j]='\0';
		char_array_4[0]=(char_array_3[0]&0xfc)>>2;
		char_array_4[1]=((char_array_3[0]&0x03)<<4)+((char_array_3[1]&0xf0)>>4);
		char_array_4[2]=((char_array_3[1]&0x0f)<<2)+((char_array_3[2]&0xc0)>>6);
		char_array_4[3]=char_array_3[2]&0x3f;
		for(j=0;j<i+1;++j)
			*buf++=base64_chars[char_array_4[j]];
		while((i++)<3)
			*buf++='=';
	}

	return cs;
}

std::string base64_decode(std::string const& encoded_string){
	int in_len=encoded_string.size();
	int i=0;
	int j=0;
	int in_=0;
	unsigned char char_array_4[4],char_array_3[3];
	std::string ret;

	while((in_len--)&&(encoded_string[in_]!='=')&&is_base64(encoded_string[in_])){
		char_array_4[i++]=encoded_string[in_]; 
		++in_;
		if(i==4){
			for(i=0;i<4;++i)
				char_array_4[i]=base64_chars.find(char_array_4[i]);
			char_array_3[0]=(char_array_4[0]<<2)+((char_array_4[1]&0x30)>>4);
			char_array_3[1]=((char_array_4[1]&0xf)<<4)+((char_array_4[2]&0x3c)>>2);
			char_array_3[2]=((char_array_4[2]&0x3)<<6)+char_array_4[3];

			for(i=0;i<3;++i)
				ret+=char_array_3[i];
			i=0;
		}
	}

	if(i){
		for(j=i;j<4;++j)
			char_array_4[j]=0;
		for(j=0;j<4;++j)
			char_array_4[j]=base64_chars.find(char_array_4[j]);

		char_array_3[0]=(char_array_4[0]<<2)+((char_array_4[1]&0x30)>>4);
		char_array_3[1]=((char_array_4[1]&0xf)<<4)+((char_array_4[2]&0x3c)>>2);
		char_array_3[2]=((char_array_4[2]&0x3)<<6)+char_array_4[3];

		for(j=0;j<i-1;++j) 
			ret+=char_array_3[j];
	}

	return ret;
}


