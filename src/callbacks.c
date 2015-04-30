#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
#include<netdb.h>
#include<dlfcn.h>

#define PPORT 110
#define PORT 25
#define SIZE 1024
//Mail commands
#define EHLO 0
#define AUTH 1
#define USER 2
#define PASS 3
#define MAIL 4
#define RCPT 5
#define DATA 6
#define CONT 7
#define QUIT 8
//pop3 commands
#define USERR 0
#define PASSS 1
#define STAT 2
#define LIST 3
#define RETR 4
#define DELE 5
#define QUITT 6

#define SOFILE "/root/MyMail/src/mylib.so"

int (*Get_last_number)(int i);
int (*Is_have_attachment)(int f_number,int model);
int (*Get_information)(int f_number,
		       char* date,
		       char* my_address,
		       char* he_address,
		       char* mail_content,
		       char* subject,int model);
int (*Get_attachment_information)(int f_number,
				  char* date,
				  char* my_address,
				  char* he_address,
				  char* mail_content,
				  char* subject,
				  char* attachment_subject,int model);
void (*Get_attachment)(int file_number,char* attachment_way,int model);
void (*Mail_delete)(int f_number,int model);
int (*Out_text_write)(char* my_address,
		      char* he_address,
		      char* subject,
		      char* text_content,
		      char* pop_file_name);
int (*Out_attachment_write)(char* my_address,
			    char* he_address,
			    char* subject,
			    char* text_content,
			    char* out_attachment_file_name,
			    char* pop_file_name);

/* bind function of C language*/
void addfunc()
{
	void* dp;
	dp =dlopen(SOFILE,RTLD_LAZY);
	Get_last_number = (int(*)(int i))dlsym(dp,"Get_last_number");
	Is_have_attachment = (int(*)(int f_number,int model))dlsym(dp,"Is_have_attachment");
	Get_information = (int(*)(int,char*,char*,char*,char*,char*,int))dlsym(dp,"Get_information");
	Get_attachment_information = (int(*)(int,char*,char*,char*,char*,char*,char*,int))dlsym(dp,"Get_attachment_information");
	Get_attachment = (void(*)(int,char*,int))dlsym(dp,"Get_attachment");
	Mail_delete = (void(*)(int,int))dlsym(dp,"Mail_delete");
	Out_text_write = (int(*)(char*,char*,char*,char*,char*))dlsym(dp,"Out_text_write");
	Out_attachment_write = (int(*)(char*,char*,char*,char*,char*,char*))dlsym(dp,"Out_attachment_write");
	
}


char storepassword[1024];
char storeaddress[1024];
char storename[1024];
char storesmtp[1024];
char storepop[1024];
char storeattachment[1024];
char* address;
char* password;
char* name;
char* smtp;
char* pop;
int mailmode = 0;
int globalmode = 1;
int mailindex = 0;
char oridir[100];
GtkWidget* FrameSend;
GtkWidget* FrameAccount;
/* x^y */
int pow(int x,int y)
{
	int i = 0;
	int total = 1;
	for(i = 0; i < y; i++)
	{
		total = total * x;
	}
	return total;
}

/* char to int*/
int toInt(char* s)
{
	int temp = 0;
	int len = 0;
	int i = 0;
	int tlen = 0;
	len = strlen(s);
	tlen = len;
	for(i = 0; i < len; i++)
	{
		
		temp  = temp + (s[i]-'0') * pow(10,tlen-1);
		tlen--;
	}
	return temp;
}

/*int to char*/
char* toChar(const int n)
{
	int i = 1;
	int j = 0;
	int temp = n;
	char* dest;
	while(n/i != 0)
		i = i * 10;
	i = i/10;
	dest = (char*)malloc(10*sizeof(char));
	while( i != 0)
	{
		dest[j] = temp / i + '0';
		temp = temp % i;
		i = i / 10;
		j++;
	}
	dest[j] = '\0';
	return dest;
}
//base64
void base64enc(const char* instr,char* outstr)
{
	char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int instr_len = 0,left = 0;
	unsigned char buf1[4] = "",buf2[4] ="";
	instr_len = strlen(instr);
	left = instr_len % 3;
	int divide = instr_len - left;
	int i = 0,j = 0;
	for(i = 0; i <= divide; i++)
	{
		if(i == instr_len - left)
			strncpy(buf1,instr,left);
		else
			strncpy(buf1,instr,3);
		buf2[0] = buf1[0] >> 2;
		buf2[1] = (buf1[0]&0x03) << 4 | buf1[1] >> 4;
		buf2[2] = (buf1[1]&0x0f) << 2 | buf1[2] >> 6;
		buf2[3] = buf1[2] & 0x3f;
		for(j = 0; j < 4; j++)
			buf2[j] = table[buf2[j]];
		if( i == instr_len - left)
			for( j = 3; j > left; j--)
				buf2[j] = '=';
		strncat(outstr,buf2,4);
		instr = instr + 3;
	}


}


/*pop3*/
void recvmail(char* myname,char* mypassword,char* myaddr,int mycount)
{
	FILE* fp;
	char ip_str[INET_ADDRSTRLEN];
	const char* p = NULL;
	char buff[SIZE];
	char mail[1024*1024];
	char tempbuf[1024];
	char tmp[4];
	int len[100];
	int sockfd;
	int i;
	int j;
	int sendnum;
	int msgnum;
	struct sockaddr_in serv_addr;
	struct hostent* server_ip;
	int numbytes = 0;
	int ret_val = 0;
	
	char user[512]; //user name
	strcpy(user,myname);
	char pass[512]; //mail password
	strcpy(pass,mypassword);
	char* msg[7] = {""};
	char msgNo[10];
	char* n_return[7] = {""};
	char temp[100];
	char temp2[100];
	FILE *f_open,*f_write;
	strcpy(temp,"USER ");
	msg[USERR] = (char*)malloc(512 * sizeof(char));
	strcpy(msg[USERR],strcat(strcat(temp,myname),"\r\n"));
	strcpy(temp,"PASS ");
	msg[PASSS] = (char*)malloc(512 * sizeof(char));
	strcpy(msg[PASSS], strcat(strcat(temp,mypassword),"\r\n"));

	msg[LIST] = "LIST\r\n";
	msg[STAT] = "STAT\r\n";
	msg[RETR] = (char*)malloc(10 * sizeof(char));
	strcpy(msg[RETR],"RETR ");
	msg[DELE] = (char*)malloc(10 * sizeof(char));
	strcpy(msg[DELE],"DELE ");
	msg[QUITT] = "QUIT\r\n";
	

	for(i = 0; i < 7; i++)
	{
		n_return[i] = "+OK";
	}
	
	
	//connect server and send command 
	while((server_ip = gethostbyname(myaddr)) == NULL)
	{
		printf("get hostIP error!\n");
		//sleep(300);
		return;
	}
	//socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket error");
		return;
	}
	//address info
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PPORT);
	p = inet_ntop(AF_INET,((struct in_addr*)server_ip->h_addr),ip_str,INET_ADDRSTRLEN);
	inet_pton(AF_INET,ip_str,&serv_addr.sin_addr);

	//connect
	ret_val = connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
	if(ret_val == -1)
	{
		perror("connect error");
		return;
	}

	//receive message
	if(numbytes = recv(sockfd,buff,SIZE,0) == -1)
	{
		perror("recv error");
		return;
	}
	for(i = 0; i < 4; i++)
		tmp[i] = '\0';
	strncpy(tmp,buff,3);
	if(strcmp(tmp,"+OK")!=0)
		return;
	//send message
	i = USERR;
	while( i < 4 )
	{
		numbytes = send(sockfd,msg[i],strlen(msg[i]),0);
		if(numbytes == -1)
		{
			perror("send error");
			break;
		}
		printf("%s\n",msg[i]);
		numbytes = recv(sockfd,buff,SIZE,0);
		if(numbytes == -1)
		{
			perror("receive error");
			break;
		}
		buff[numbytes] = '\0';
		if(strcmp(msg[i],"STAT\r\n") == 0)
		{

			strtok(buff," ");
			msgnum = toInt(strtok(NULL," "));
			if(msgnum == 0)
				return;
	
		}
		if(strcmp(msg[i],"LIST\r\n") == 0)
		{

			strtok(buff,"\n");
			strtok(NULL," ");
			strcpy(temp,strtok(NULL,"\n"));
		
			strcpy(temp2,temp);
			temp2[strlen(temp2)-1] = '\0';
			len[0] = toInt(temp2);
			for(j=1; j < msgnum; j++)
			{
				strcpy(temp2,"");
				strcpy(temp,"");
				strtok(NULL," ");
				strcpy(temp,strtok(NULL,"\r\n"));
				strncpy(temp2,temp,strlen(temp));
				temp2[strlen(temp)] = '\0';
				len[j] = toInt(temp2);
			}
		}
		strncpy(tmp,buff,3);
		printf("tmp %s\n",tmp);
		printf("command:%s\n",msg[i]);
		printf("return value:%s\n",buff);
		printf("right return:%s\n",n_return[i]);
		if(strcmp(tmp,n_return[i]) == 0)
		i++;
		else
		return;
	}
	printf("mycount is:%d\n",mycount);
	//receive mail
	for(j = 0; j < msgnum; j++)
	{	
		mycount ++;
		
		fp = fopen("in.txt","a+");
		sendnum = len[j] + strlen(toChar(len[j])) + 24;
	//	printf("sendnum is: %d\n",sendnum);
		strcpy(mail,"");
		strcpy(msg[RETR],"RETR ");
		strcpy(msgNo,toChar(j+1));
	
		numbytes = send(sockfd,strcat(strcat(msg[RETR],msgNo),"\r\n"),8,0);
		printf("%s\n",strcat(msg[RETR],msgNo));
		if(numbytes == -1)
		{
			perror("send error");
			break;
		}
		while(sendnum != 0)
		{
			numbytes = recv(sockfd,tempbuf,128,0);
			tempbuf[numbytes] = '\0';
			sendnum = sendnum - numbytes;
	//		printf("send num is: %d\n",sendnum);		
	//		printf("%d\n",numbytes);
	//		printf("%s\n",tempbuf);
	//		printf("%d\n",aaa);
	//		''
			fwrite(tempbuf,sizeof(char),strlen(tempbuf),fp);
		}
		strcpy(msg[DELE],"");
		strcpy(msg[DELE],"DELE ");
		numbytes = send(sockfd,strcat(strcat(msg[DELE],msgNo),"\r\n"),8,0);
		strcpy(tempbuf,"");
		numbytes = recv(sockfd,tempbuf,128,0);	
		printf("msgno:%s\ntempbuf:%s\n",msgNo,tempbuf);
		fwrite("!@#$%",sizeof(char),strlen("!@#$%"),fp);
	//	printf("mycount is:%s",toChar(mycount));
		fwrite(toChar(mycount),sizeof(char),strlen(toChar(mycount)),fp);
		fwrite("!@#$%\n",sizeof(char),strlen("!@#$%\n"),fp);
	//	printf("%d\n\n\n\n\n",numbytes);
		if(numbytes == -1)
		{
			perror("receive error");
			break;
		}
		fclose(fp);
	}
	//send quit
	numbytes = send(sockfd,msg[QUITT],strlen(msg[QUITT]),0);
	if(numbytes == -1)
	{
		perror("send error");
		exit(1);
	}
	numbytes = recv(sockfd,buff,SIZE,0);
	printf("%s\n",buff);
	close(sockfd);
}
/*smtp*/
void sendmail(gchar* myname,gchar* mypassword,gchar* mysmtp,gchar* from,gchar* to)
{
	char ip_str[INET_ADDRSTRLEN];
	const char*p = NULL;
	int i;
	char buff[SIZE];
	char tmp[4];
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent* server_ip;
	int numbytes = 0;
	int ret_val = 0;
	char mailbuf[1024];
	char username[512];  //mail user
	strcpy(username,myname);
	char password[512]; //mail password
	strcpy(password,mypassword);
	char*msg[9] = {""};
	char*n_return[9] = {""}; //return number
	FILE *fp;
	//msg[EHLO] = "ehlo pub.ss.pku.edu.cn\r\n";
        msg[EHLO] = (char*)malloc(512 * sizeof(char));
	strcpy(msg[EHLO],"ehlo ");
	strcat(msg[EHLO],mysmtp);
	strcat(msg[EHLO],"\r\n");
	printf("%s\n",msg[EHLO]);
	msg[AUTH] = "auth login\r\n";
	char user64[512] = "";
	char pass64[512] = "";
	printf("%s\n",username);
	printf("%s\n",password);
	base64enc(username,user64);
	base64enc(password,pass64);

	printf("%s\n",user64);
	printf("%s\n",pass64);
	msg[PASS] = strcat(pass64,"\r\n");
	msg[USER] = strcat(user64,"\r\n");
	//msg[MAIL] = "mail from:<10717365@pub.ss.pku.edu.cn>\r\n";
	msg[MAIL] = (char*)malloc(1024 * sizeof(char));
	strcpy(msg[MAIL],"mail from:<");
	strcat(msg[MAIL],(char*)from);
	strcat(msg[MAIL],">\r\n");
	printf("%s\n",msg[MAIL]);
	//msg[RCPT] = "rcpt to:<10717169@pub.ss.pku.edu.cn>\r\n";
	msg[RCPT] = (char*)malloc(1024 * sizeof(char));
	
	strcpy(msg[RCPT],"rcpt to:<");
	strcat(msg[RCPT],to);
	strcat(msg[RCPT],">\r\n");
	printf("%s\n",msg[RCPT]);
	msg[DATA] = "data\r\n";
	msg[CONT] = "\r\n.\r\n";
	msg[QUIT] = "quit\r\n";

	n_return[EHLO] = "250";
	n_return[AUTH] = "334";
	n_return[USER] = "334";
	n_return[PASS] = "235";
	n_return[MAIL] = "250";
	n_return[RCPT] = "250";
	n_return[DATA] = "354";
	n_return[CONT] = "250";
	printf("%s\n",mysmtp);
	//connect server and send command
	while((server_ip = gethostbyname(mysmtp)) == NULL)
	{
		printf("gethostbyname error");
		sleep(300);
	}
	//socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket error");
		exit(1);
	}
	//address info
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	p = inet_ntop(AF_INET,((struct in_addr*)server_ip->h_addr),ip_str,INET_ADDRSTRLEN);
	inet_pton(AF_INET,ip_str,&serv_addr.sin_addr);
//	bzero(&(serv_addr.sin_zero),8);
	
	//connect
	ret_val = connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
	if(ret_val == -1)
	{
		perror("connect error");
		exit(1);
	}
	//receive message
	if(numbytes=recv(sockfd,buff,SIZE,0) == -1)
	{
		perror("recv error");
		exit(1);
	}
	printf("%s\n",buff);
	for(i = 0; i < 4; i++)
		tmp[i] = '\0';
	strncpy(tmp,buff,3);
	if(strcmp(tmp,"220")!=0)
		exit(1);
	//send message
	i = EHLO;
	while(i < 8)
	{
		numbytes = send(sockfd,msg[i],strlen(msg[i]),0);
		if(numbytes == -1)
		{
			perror("send error");
			break;
		}
		printf("%d %s\n\n\n\n",i,msg[i]);
		numbytes = recv(sockfd,buff,SIZE,0);
		if(numbytes == -1)
		{
			perror("recv error");
			break;
		}
		if(strcmp(msg[i],"data\r\n") == 0)
		{
			fp = fopen("smtp","r");
			printf("0000000000000000000000000000\n");
			while(1)
			{
				fgets(mailbuf,1024,fp);
				send(sockfd,mailbuf,strlen(mailbuf),0);
				if(feof(fp))
				break;
			}
		}
	//	if(strcmp(msg[i],".\r\n") != 0)
		strncpy(tmp,buff,3);
		printf("tmp:%s\n",tmp);
		printf("command:%s\n",msg[i]);
		printf("return value:%s\n",buff);
		printf("right return:%s\n",n_return[i]);
		if(strcmp(tmp,n_return[i]) == 0)
		i++;
		else
		break;
	}
	
	//send quit
	numbytes = send(sockfd,msg[QUIT],strlen(msg[QUIT]),0);
	if(numbytes == -1)
	{
		perror("send error");
		return -1;
	}
	
	close(sockfd);
	

}

void
on________n_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________o_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________s_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on___________a_1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________q_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________t_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________c_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________p_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on________d_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

/* about */
void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* win;
	win = create_FrmAbout();
	gtk_widget_show(win);
}



/* call window2*/
void
on______________o_1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{	GtkWidget* win2;
	char dirname[100];
	getcwd(dirname,sizeof(dirname));
	//strcpy(oridir,dirname);
	strcpy(oridir,"/root/MyMail/src");
	win2 = create_window2();
	gtk_widget_show(win2);


}

/* call frmaccount*/
void
on______________g_1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/* account window*/
	GtkWidget* lst;
	GtkWidget* win;
	int cc;
	strcpy(oridir,"/root/MyMail/src");
	chdir(oridir);
	win = create_FrmAccount();
	FrameAccount = win;

	lst = lookup_widget(win,"lst_account");
	char* name[10][1] ={""};
	int i = 0,j = 0;
	for(i = 0; i < 10; i++)
	{
		name[i][0] = (char*)malloc(100 * sizeof(char));
		strcpy(name[i][0],"");
	}
	gtk_clist_clear((GtkCList*)lst);
	cc = AccountBind(name);
	for(i = 0; i < cc; i++)
	{
		gtk_clist_append((GtkCList*)lst,name[i]);
	}
	for(i = 0; i < 10; i++)
	{
		free(name[i][0]);
		name[i][0] = NULL;
	}

	gtk_widget_show(win);
}

/*bind account*/
int AccountBind(char* name[][1])
{
	FILE* fp;
	char buffer[100];
	int i = 0;
	fp = fopen("profile.txt","r");
	while(fgets(buffer,100,fp))
	{
		if(isspace(buffer[0]))
		break;
		buffer[strlen(buffer)-1] = '\0';
		strcpy(name[i][0],buffer);
	
		i++;
	}
	fclose(fp);
	printf("i is: %d\n",i);
	return i;
}

/* create sendframe*/
void
on______________n_1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget* sf;
	sf = create_SendFrame();
	FrameSend = sf;
	gtk_widget_show(sf);

}


void
on_btn_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* taddress;
	GtkWidget* tname;
	GtkWidget* tsmtp;
	GtkWidget* tpop;
	GtkWidget* tpassword;
	GtkWidget* win2;
	char exist[1024];
	FILE* fp;
	char c;
	char command[100];
	win2 = lookup_widget(button,"window2");
	taddress = lookup_widget(button,"txb_address");
	tname = lookup_widget(button,"txb_name");
	tsmtp = lookup_widget(button,"txb_smtp");
	tpop = lookup_widget(button,"txb_pop");
	tpassword = lookup_widget(button,"txb_password");
	address = gtk_entry_get_text(GTK_ENTRY(taddress));
	name = gtk_entry_get_text(GTK_ENTRY(tname));
	smtp = gtk_entry_get_text(GTK_ENTRY(tsmtp));
	pop = gtk_entry_get_text(GTK_ENTRY(tpop));
	password = gtk_entry_get_text(GTK_ENTRY(tpassword));
	strcpy(storeaddress,address);
	strcpy(storename,name);
	strcpy(storepop,pop);
	strcpy(storesmtp,smtp);
	strcpy(storepassword,password);
	getcwd(command,sizeof(command));
	//strcpy(oridir,command);

	if(Validate(storename,exist,oridir) == 1)
	{
		getcwd(command,sizeof(command));
		strcat(command,"/");
		strcat(command,storename);
		printf("The current dir is: %s\n",command);
		chdir(command);
		gtk_widget_destroy(win2);
		return;
	}
	strcpy(command,"mkdir ");
	strcat(command,storename);
	system(command);
	fp = fopen("profile.txt","a+");
	fwrite(storename,sizeof(char),strlen(storename),fp);
	fwrite("\n",sizeof(char),strlen("\n"),fp);
	fclose(fp);
	getcwd(command,sizeof(command));
	strcat(command,"/");
	strcat(command,storename);
	printf("%s\n",command);
	
	chdir(command);
	system("> in.txt");
	system("> in_delete.txt");
	fp = fopen("in.txt","a+");
	fwrite("!@#$%1!@#$%",sizeof(char),strlen("!@#$%1!@#$%"),fp);
	fclose(fp);
	system("> out.txt");
	system("> out_delete.txt");
	fp = fopen("out.txt","a+");
	fwrite("!@#$%1!@#$%",sizeof(char),strlen("!@#$%1!@#$%"),fp);
	fclose(fp);
	WriteInfo(storename,storeaddress,storepassword,storesmtp,storepop);
	g_print("%s %s %s %s \n",address,name,smtp,pop);
	GtkWidget* tempwin;
	GtkWidget* templb;
	tempwin = create_dg_instruction();
	templb = lookup_widget(tempwin,"lb_message");
	gtk_label_set_text((GtkLabel*)templb,"添加用户成功");
	gtk_widget_show(tempwin);
	gtk_widget_destroy(win2);

}
/*write userinfo in myinfo.txt */
void WriteInfo(char* myname,char* myaddress,char* mypassword,char* mysmtp,char* mypop)
{
	FILE* fp;
	fp = fopen("myinfo.txt","w+");

	fwrite(myaddress,sizeof(char),strlen(myaddress),fp);
	fwrite("\n",sizeof(char),strlen("\n"),fp);
	fwrite(myname,sizeof(char),strlen(myname),fp);
	fwrite("\n",sizeof(char),strlen("\n"),fp);
	fwrite(mypassword,sizeof(char),strlen(mypassword),fp);
	fwrite("\n",sizeof(char),strlen("\n"),fp);
	fwrite(mysmtp,sizeof(char),strlen(mysmtp),fp);
	fwrite("\n",sizeof(char),strlen("\n"),fp);
	fwrite(mypop,sizeof(char),strlen(mypop),fp);
	fwrite("\n",sizeof(char),strlen("\n"),fp);
	fclose(fp);
}
/* validate if the user existed*/
int Validate(char* myname,char* myexist,char* comm)
{	
	char mydir[1024];
	int i = 0; 
	FILE* fp;
	printf("%s\n",comm);
	//fp = fopen("profile.txt","r");
	//if(fp == NULL)
	//{
		chdir(comm);

		fp = fopen("profile.txt","r");
	//}
	while(1)
	{
		fgets(myexist,1024,fp);
		if(strncmp(myexist,storename,strlen(storename)) == 0)
		{	
			printf("Account exist!\n");	
			i = 1;
			break;
		}
		if(feof(fp))
			break;
	}
	return i;
}

void
on_btn_cancle_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btn_send_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{	
	char* buf;
	GtkTextBuffer* buffer;
	char* myto;
	char* bufsubject;
	GtkTextIter start,end;
	GtkWidget* tcontent;
	GtkWidget* tto;
	GtkWidget* tsubject;
	tto = lookup_widget(button,"txb_to");
	myto = gtk_entry_get_text(GTK_ENTRY(tto));
	tsubject = lookup_widget(button,"txb_subject"); 
	bufsubject = gtk_entry_get_text(GTK_ENTRY(tsubject));
	tcontent = lookup_widget(button,"tv_content");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tcontent));
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffer),&start,&end);
	const GtkTextIter s = start,e = end;
	buf = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer),&s,&e,FALSE);
	printf("%s\n",buf);

	if(mailmode == 0)
	{
		Out_text_write(storeaddress,myto,bufsubject,buf,"smtp");
	}
	else
	{
		printf("%s",storeattachment);
		Out_attachment_write(storeaddress,myto,bufsubject,buf,storeattachment,"smtp");
	}

	sendmail(storename,storepassword,storesmtp,storeaddress,myto);
	strcpy(storeattachment,"");
	mailmode = 0;
	GtkWidget* tempwin;
	GtkWidget* templb;
	tempwin = create_dg_instruction();
	templb = lookup_widget(tempwin,"lb_message");
	gtk_label_set_text((GtkLabel*)templb,"发送邮件成�?);
	gtk_widget_show(tempwin);

	//recvmail(storename,storepassword,storesmtp);	
}


void
on_MainFrame_destroy                   (GtkObject       *object,
                                        gpointer         user_data)
{
	exit(1);
}


void
on_btn_attachment_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* attachment;
	attachment = create_fs_attachment();
	gtk_widget_show(attachment);

}


void
on_fs_attachment_btn_ok_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	char* temp;
	temp = (char*)malloc(100 * sizeof(char));
	GtkWidget* Myfile;
	GtkWidget* Father;
	GtkWidget* attachment;
	Myfile = lookup_widget(button,"fs_attachment");
	temp = gtk_file_selection_get_filename(GTK_FILE_SELECTION(Myfile));
	printf("temp is:%s\n",temp);
	mailmode = 1;
	strcpy(storeattachment,temp);
//	Father = lookup_widget(Myfile//gtk_widget_get_parent_window(Myfile);
	attachment = lookup_widget(FrameSend,"txb_attach");
	gtk_entry_set_text(GTK_ENTRY(attachment),temp);

	gtk_widget_destroy(Myfile);
}


void
on_btn_update_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	int i;
	i = Get_last_number(1);
	printf("i is: %d\n",i);
/*	strcpy(storename,"10717365");
	strcpy(storepassword,"6188699");
	strcpy(storesmtp,"pub.ss.pku.edu.cn"); */
	recvmail(storename,storepassword,storesmtp,i);	
}


void
on_ls_box_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	int cc;
	int i;
	int j;
	GtkWidget* mylist;
	char* in[20][5] = {""};
	char* out[20][5] = {""};
	mylist = lookup_widget(clist,"ls_in");
	for(i=0; i < 20; i++)
	{
		for(j=0; j < 5; j++)
		{
			in[i][j] = (char*)malloc(1024 * sizeof(char));
			out[i][j] = (char*)malloc(1024 * sizeof(char));
		}
	}
	gtk_clist_clear((GtkCList*)mylist);
	if(row == 0)
	{
		globalmode = 1;
		cc = InBind(in);
		for(i=0; i < cc-1; i++)
		{
			gtk_clist_append((GtkCList*)mylist,in[i]);
		}

	}
	if(row == 1)
	{
		globalmode = 0;
		cc = OutBind(out);
		for(i=0; i < cc-1; i++)
		{
			gtk_clist_append((GtkCList*)mylist,out[i]);
		}
	}
}

int InBind(char* in[][5])
{
	int mailnum;
	int i;
	char date[1024];
	char my_address[1024];
	char he_address[1024];
	char mail_content[1024*1024];
	char subjects[1024];
	char attachment_subject[1024];
	mailnum = Get_last_number(1);
	int j = 0;
	for(i = 0; i < mailnum; i++)
	{
		if(Is_have_attachment(i+1,1)==1)
		{
			if(Get_attachment_information(i+1,date,my_address,he_address,mail_content,subjects,attachment_subject,1) == 1)
			{
				strcpy(in[j][0],toChar(i+1));
				strcpy(in[j][1],he_address);
				strcpy(in[j][2],subjects);
				strcpy(in[j][3],date);
				strcpy(in[j][4],"�?);
				printf("subject is:%s\n",subjects);
				printf("%s\n%s\n%s\n%s\n",in[j][0],in[j][1],in[j][2],in[j][3]);
			}
			else
				continue;
		}
		else
		{
			printf("else:%d\n\n\n",i);
			if(Get_information(i+1,date,my_address,he_address,mail_content,subjects,1) == 1)
			{
				strcpy(in[j][0],toChar(i+1));
				strcpy(in[j][1],he_address);
				strcpy(in[j][2],subjects);
				strcpy(in[j][3],date);
				strcpy(in[j][4],"�?);
				printf("subject is:%s\n",subjects);
				printf("%s\n%s\n%s\n%s\n",in[j][0],in[j][1],in[j][2],in[j][3]);
			}
			else
				continue;
		}
		
		j++;
	}	
	return j;
}

int OutBind(char* out[][5])
{
	int mailnum;
	int i;
	char date[1024];
	char my_address[1024];
	char he_address[1024];
	char mail_content[1024*1024];
	char subject[1024];
	char attachment_subject[1024];
	mailnum = Get_last_number(0);
	int j = 0;
	for(i=0; i < mailnum; i++)
	{
		if(Is_have_attachment(i+1,0)==1)
		{
			if(Get_attachment_information(i+1,date,my_address,he_address,mail_content,subject,attachment_subject,0) == 1)
			{
				strcpy(out[j][0],toChar(i+1));
				strcpy(out[j][1],my_address);
				strcpy(out[j][2],subject);
				strcpy(out[j][3],date);
				strcpy(out[j][4],"�?);
			}
			else
				continue;
		}
		else
		{
			if(Get_information(i+1,date,my_address,he_address,mail_content,subject,0) == 1)
			{
				strcpy(out[j][0],toChar(i+1));
				strcpy(out[j][1],my_address);
				strcpy(out[j][2],subject);
				strcpy(out[j][3],date);
				strcpy(out[j][4],"�?);
			}
			else
				continue;
		}
		
		j++;
	}	
		
	return j;
}

void
on_ls_in_select_row                    (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	int index;
	char* ic;
	GtkWidget* myfrom;
	GtkWidget* myto;
	GtkWidget* mydate;
	GtkWidget* mysubject;
	GtkWidget* mycontent;
	GtkTextBuffer* buffer;
	char* date = (char*)malloc(1024 * sizeof(char));
	char* my_address =  (char*)malloc(1024 * sizeof(char));
	char* he_address = (char*)malloc(1024 * sizeof(char)); 
	char* mail_content = (char*)malloc(1024 *1024 * sizeof(char));
	char* subject = (char*)malloc(1024 * sizeof(char));
	char* attachment_subject = (char*)malloc(1024 * sizeof(char));
	int mode = globalmode;
	gtk_clist_get_text(GTK_CLIST(clist),row,0,&ic);
	index = toInt(ic);
	mailindex = index;
	myfrom = lookup_widget(clist,"lb_from");
	myto = lookup_widget(clist,"lb_to");
	mydate = lookup_widget(clist,"lb_date");
	mysubject = lookup_widget(clist,"lb_subject");
	mycontent = lookup_widget(clist,"tv_gcontent");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mycontent));
	if(Is_have_attachment(index,mode)==1)
	{
		if(Get_attachment_information(index,date,my_address,he_address,mail_content,subject,attachment_subject,mode) == 1)
		{
			gtk_label_set_text((GtkLabel*)myfrom,he_address);
			gtk_label_set_text((GtkLabel*)myto,my_address);
			gtk_label_set_text((GtkLabel*)mydate,date);
			gtk_label_set_text((GtkLabel*)mysubject,subject);
			gtk_text_buffer_set_text((GtkTextBuffer*)buffer,mail_content,strlen(mail_content));
			gtk_text_view_set_buffer((GtkTextView*)mycontent,(GtkTextBuffer*)buffer);
			
		}
	}

	else
	{
		if(Get_information(index,date,my_address,he_address,mail_content,subject,mode) == 1)
		{
			gtk_label_set_text((GtkLabel*)myfrom,he_address);
			gtk_label_set_text((GtkLabel*)myto,my_address);
			gtk_label_set_text((GtkLabel*)mydate,date);
			gtk_label_set_text((GtkLabel*)mysubject,subject);
			gtk_text_buffer_set_text((GtkTextBuffer*)buffer,mail_content,strlen(mail_content));
			gtk_text_view_set_buffer((GtkTextView*)mycontent,(GtkTextBuffer*)buffer);

		}
	}

}


void
on_btn_detail_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	int index = 0;
	int mode = 1;
	GtkWidget* sendframe;
	GtkWidget* myto;
	GtkWidget* mysubject;
	GtkWidget* myattachment;
	GtkWidget* mycontent;
	GtkTextBuffer* buffer;
	GtkWidget* templb;
	char* date = (char*)malloc(1024 * sizeof(char));
	char* my_address =  (char*)malloc(1024 * sizeof(char));
	char* he_address = (char*)malloc(1024 * sizeof(char)); 
	char* mail_content = (char*)malloc(1024 *1024 * sizeof(char));
	char* subject = (char*)malloc(1024 * sizeof(char));
	char* attachment_subject = (char*)malloc(1024 * sizeof(char));
	index = mailindex;
	mode = globalmode;
	sendframe = create_SendFrame();
	templb = lookup_widget(sendframe,"label24");
	gtk_label_set_text((GtkLabel*)templb,"发件�?");
	myto = lookup_widget(sendframe,"txb_to");
	mysubject = lookup_widget(sendframe,"txb_subject");
	myattachment = lookup_widget(sendframe,"txb_attach");
	mycontent = lookup_widget(sendframe,"tv_content");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mycontent));
	if(Is_have_attachment(index,mode)==1)
	{
		if(Get_attachment_information(index,date,my_address,he_address,mail_content,subject,attachment_subject,mode) == 1)
		{
			//gtk_entry_set_text((GtkEntry*)myfrom,my_address);
			gtk_entry_set_text((GtkEntry*)myto,he_address);
			//gtk_entry_set_text((GtkEntry*)mydate,date);
			gtk_entry_set_text((GtkEntry*)mysubject,subject);
			gtk_text_buffer_set_text((GtkTextBuffer*)buffer,mail_content,strlen(mail_content));
			//printf("%s\n",buffer);
			gtk_text_view_set_buffer((GtkTextView*)mycontent,(GtkTextBuffer*)buffer);
			gtk_entry_set_text((GtkEntry*)myattachment,attachment_subject);
			
		}
	}

	else
	{
		if(Get_information(index,date,my_address,he_address,mail_content,subject,mode) == 1)
		{
			//gtk_entry_set_text((GtkEntry*)myfrom,my_address);
			gtk_entry_set_text((GtkEntry*)myto,he_address);
			//gtk_entry_set_text((GtkEntry*)mydate,date);
			gtk_entry_set_text((GtkEntry*)mysubject,subject);
			gtk_text_buffer_set_text((GtkTextBuffer*)buffer,mail_content,strlen(mail_content));
			gtk_text_view_set_buffer((GtkTextView*)mycontent,(GtkTextBuffer*)buffer);

		}
	}

	gtk_widget_show(sendframe);
	
}


void
on_btn_download_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* attachment;
	attachment = create_fs_download();
	gtk_widget_show(attachment);

}


void
on_fs_download_btn_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	char* temp;
	int mode;
	int index;
	temp = (char*)malloc(100 * sizeof(char));
	index = mailindex;
	mode = globalmode;
	GtkWidget* Myfile;
	Myfile = lookup_widget(button,"fs_download");
	temp = gtk_file_selection_get_filename(GTK_FILE_SELECTION(Myfile));
	strcat(temp,"/");
	Get_attachment(index,temp,mode);
	GtkWidget* tempwin;
	GtkWidget* templb;
	tempwin = create_dg_instruction();
	templb = lookup_widget(tempwin,"lb_message");
	gtk_label_set_text((GtkLabel*)templb,"下载附件成功");
	gtk_widget_show(tempwin);

	free(temp);
	temp = NULL;
	gtk_widget_destroy(Myfile);	
}


void
on_lst_account_select_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	GtkWidget* myaccount;
	GtkWidget* myaddress;
	GtkWidget* mypassword;
	GtkWidget* mysmtp;
	GtkWidget* mypop;
	char* strname;
	char* straddress;
	char* strpassword;
	char* strsmtp;
	char* strpop;
	char* option;
	char* mydir;
/*	int cc = 0;
	gtk_clist_clear((GtkCList*)clist);

	chdir(oridir);
	char* name[10][1] ={""};
	int i = 0,j = 0;
	for(i = 0; i < 10; i++)
	{
		name[i][0] = (char*)malloc(100 * sizeof(char));
		strcpy(name[i][0],"");
	}
	//gtk_clist_clear((GtkCList*)clist);
	cc = AccountBind(name);
	printf("cc is:%d\n",cc);
	for(i = 0; i < cc; i++)
	{
	//	gtk_clist_append((GtkCList*)clist,name[i]);
	} */
	
	
	mydir = (char*)malloc(1024 * sizeof(char));
	option = (char*)malloc(1024 * sizeof(char));
	strname = (char*)malloc(1024 * sizeof(char));
	straddress = (char*)malloc(1024 * sizeof(char));
	strpassword = (char*)malloc(1024 * sizeof(char));
	strsmtp = (char*)malloc(1024 * sizeof(char));
	strpop = (char*)malloc(1024 * sizeof(char));
	myaccount = lookup_widget(clist,"lb_account_name");
	myaddress = lookup_widget(clist,"lb_account_address");
	mypassword = lookup_widget(clist,"lb_account_password");
	mysmtp = lookup_widget(clist,"lb_account_smtp");
	mypop = lookup_widget(clist,"lb_account_pop");
	strcpy(option,"");
	printf("row is:%d\n",row);
	
	gtk_clist_get_text(clist,row,0,&option);
	printf("option is:%s\n",option);
	strcpy(mydir,"");
	strcat(mydir,oridir);
	strcat(mydir,"/");
	strcat(mydir,option);
	printf("the current dir is:%s\n",mydir);
	chdir(mydir);
	strcpy(strname,"");
	strcpy(straddress,"");
	strcpy(strpassword,"");
	strcpy(strsmtp,"");
	strcpy(strpop,"");
	GetAccount(strname,straddress,strpassword,strsmtp,strpop);
	strcpy(storename,strname);
	strcpy(storeaddress,straddress);
	strcpy(storepassword,strpassword);
	strcpy(storesmtp,strsmtp);
	strcpy(storepop,strpop);
	gtk_label_set_text((GtkLabel*)myaccount,strname);
	gtk_label_set_text((GtkLabel*)myaddress,straddress);
	gtk_label_set_text((GtkLabel*)mypassword,"*******");
	gtk_label_set_text((GtkLabel*)mysmtp,strsmtp);
	gtk_label_set_text((GtkLabel*)mypop,strpop);
	


}

/* read into*/
void GetAccount(char* myname,char* myaddress,char* mypass,char* mysmtp,char* mypop)
{
	FILE* fp;
	fp = fopen("myinfo.txt","r");
	fgets(myaddress,1024,fp);
	myaddress[strlen(myaddress)-1] = '\0';
	fgets(myname,1024,fp);
	myname[strlen(myname)-1] = '\0';
	fgets(mypass,1024,fp);
	mypass[strlen(mypass)-1] = '\0';
	fgets(mysmtp,1024,fp);
	mysmtp[strlen(mysmtp)-1] = '\0';
	fgets(mypop,1024,fp);
	mypop[strlen(mypop)-1] = '\0';
	fclose(fp);
}


void
on_btn_account_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* win;
	win = lookup_widget(button,"FrmAccount");
	GtkWidget* tempwin;
	GtkWidget* templb;
	tempwin = create_dg_instruction();
	templb = lookup_widget(tempwin,"lb_message");
	gtk_label_set_text((GtkLabel*)templb,"更改用户成功");
	gtk_widget_show(tempwin);

	gtk_widget_destroy(win);
}


void
on_btn_delete_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	int mode;
	int index;
	char* option;
	FILE* fp;
	index = mailindex;
	mode = globalmode;
	if(mode == 1)
	{
		fp = fopen("in_delete.txt","a+");
		fwrite(toChar(index),sizeof(char),strlen(toChar(index)),fp);
		fwrite("\n",sizeof(char),strlen("\n"),fp);
	}
	if(mode == 0)
	{
		fp = fopen("out_delete.txt","a+");
		fwrite(toChar(index),sizeof(char),strlen(toChar(index)),fp);
		fwrite("\n",sizeof(char),strlen("\n"),fp);
	}
	GtkWidget* tempwin;
	GtkWidget* templb;
	tempwin = create_dg_instruction();
	templb = lookup_widget(tempwin,"lb_message");
	gtk_label_set_text((GtkLabel*)templb,"邮件删除成功");
	gtk_widget_show(tempwin);

	fclose(fp);
		
		
}


void
on_btn_dlg_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget* win;
	win = lookup_widget(button,"dg_instruction");
	gtk_widget_destroy(win);
}

