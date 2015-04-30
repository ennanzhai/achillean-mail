CC=g++

all:test

test:Handle_out_mail.o test.o base64.o string_handle.o File_handle.o Handle_in_mail.o

test.o:test.cpp Handle_out_mail.h Handle_in_mail.h File_handle.h

Handle_out_mail.o:Handle_out_mail.cpp base64.o string_handle.o

Handle_in_mail.o:Handle_in_mail.cpp base64.o string_handle.o

string_handle.o:string_handle.cpp

base64.o:base64.cpp

File_handle.o:File_handle.cpp string_handle.o

clean:
	rm test