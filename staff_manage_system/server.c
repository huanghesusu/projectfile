#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "common.h"
#include <sqlite3.h> 

/* 初始化数据库      */
Type database_init(void){
	sqlite3 *db;
    char *errmsg = NULL;
	char **resultp;
    int nrow = 0;
	int ncolumn = 0;
	char sql[128];
	/* 创建(打开)数据库 */
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));		
		return FALSE;
	}	
	/* 创建数据库列表*/
	sqlite3_exec(db,"create table staff(number int,name char,type int,password char,age int,salary int);",NULL,NULL,&errmsg);
	sprintf(sql,"select * from staff where name='renzhengfei';");
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncolumn,&errmsg) != 0){	
		fprintf(stderr,"%s\n",errmsg);
		return FALSE;
	}
    if(nrow == 0){
		/* 创建管理员用户 */
		if(sqlite3_exec(db,"insert into staff values(1000,'renzhengfei',0,1001,75,0);",NULL,NULL,&errmsg) != 0)
		{
			fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
			return FALSE;	
		}
    }	
	sqlite3_close(db);	
	return OK;
}
/* 线程函数 */
void *handle(void *arg){
	T_LogIn login_info;
	int acceptfd = *(int*)arg;
	if(pthread_detach(pthread_self()) != 0){
		perror("pthread_detach");
		return NULL;
	}
	ssize_t t;
	t = recv(acceptfd,&login_info,sizeof(login_info),0);
	if(t == -1){
		perror("recv");
		exit(1);
	}
    /* 匹配 */
    if(do_client_login(&login_info) == OK){
		if(login_info.type  == ADMIN){
			strcpy(login_info.msg,"Login OK\nWelcome to the administrator system");
			send(acceptfd,&login_info,sizeof(login_info),0);
		}
		else if(login_info.type == USER){
			strcpy(login_info.msg,"Login OK\nWelcome to the staff system");
			send(acceptfd,&login_info,sizeof(login_info),0);
		}
	}
    else{
		strcpy(login_info.msg,"name or password or type is wrong");
		send(acceptfd,&login_info,sizeof(login_info),0);
	}
	do_client_request(acceptfd);	
	close(acceptfd);
	free(arg);
	arg = NULL;
	return NULL;
}

Type do_client_login(PT_LogIn login_info){
	sqlite3 *db;
    char **resultp;
    char *errmsg = NULL;
    int nrow = 0;
	int ncolumn = 0;
	char sql[128];
	if(sqlite3_open("StaffManage.db",&db)!=0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}	
	sprintf(sql,"select * from staff where name='%s' and type=%d and password='%s';",\
	        login_info->name,login_info->type,login_info->password);
    printf("%s\n",sql);
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncolumn,&errmsg) != 0){	
		fprintf(stderr,"%s\n",errmsg);
		return FALSE;
	}	
    if(nrow == 1){
		sqlite3_close(db);
        return OK;
	}
	else{
		sqlite3_close(db);
		return FALSE;
	}	
	return OK;
}

Type do_client_request(int acceptfd){
	
    T_StaffInfo staff_info = {0};
	while(recv(acceptfd, &staff_info, sizeof(staff_info), 0) > 0)
	{
	   printf("request_type:%d\n", staff_info.request_type);
	   //printf("password:%s\n",staff_info.password);
	   switch(staff_info.request_type)
	   {
	  	 case ADMINNAME:
			  do_admin_query_forname(acceptfd, &staff_info);
			  break;
		 case QUERYALL:
			  do_admin_query_forall(acceptfd, &staff_info);
			  break;
		 case ADMINMODIFY:
			  do_admin_modify(acceptfd, &staff_info);
			  break;
		 case ADMINADD:
		 	  do_admin_add(acceptfd, &staff_info);
              break;
		 case ADMINDEL:
		 	  do_admin_del(acceptfd, &staff_info);
			  break;
		 case ADMINHISTORY:
		 	  do_admin_history(acceptfd, &staff_info);
			  break;
		 case USERQUERY:
		 	  do_user_query(acceptfd, &staff_info);
			  break;
		 case USERMODIFY:
			  do_user_modify(acceptfd, &staff_info);
			  break;
		 default:
			  printf("Invalid data msg.\n");
			  break;			 
	   }
	}
	return OK;
}

Type do_admin_query_forname(int acceptfd, PT_StaffInfo staff_info){
	sqlite3 *db;
    char **resultp;
    char *errmsg = NULL;
    int nrow = 0;
	int ncolumn = 0;
	char sql[128];
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}	
	sprintf(sql,"select * from staff where name='%s';",staff_info->name);
    printf("%s\n",sql);
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncolumn,&errmsg)!=0){	
		fprintf(stderr,"%s\n",errmsg);
		return FALSE;
	}	
    if(nrow == 1){
		//strcpy(staff_info->msg,"query successfulelly");
		//send(acceptfd,&login_info,sizeof(login_info),0);		
		int i,j,num = 0;	
			for(i = 0;i < nrow+1;i++)
			{
				for(j = 0;j < ncolumn;j++)
				{
					strcat(staff_info->msg,resultp[num++]);
					strcat(staff_info->msg,"      ");
				}
				strcat(staff_info->msg,"\n=================================================================\n");
			}
		send(acceptfd,staff_info,sizeof(T_StaffInfo),0);
		sqlite3_close(db);
        return OK;
	}
	else{
		sqlite3_close(db);
		return FALSE;
	}	
	return OK;
}

Type do_admin_query_forall(int acceptfd, PT_StaffInfo staff_info){
    sqlite3 *db;
    char **resultp;
    char *errmsg = NULL;
    int nrow = 0;
	int ncolumn = 0;
	char sql[128];
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}	
	sprintf(sql,"select * from staff;");
    printf("%s\n",sql);
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncolumn,&errmsg)!=0){	
		fprintf(stderr,"%s\n",errmsg);
		return FALSE;
	}
	int i,j,num = 0;		
	for(i = 0;i < nrow + 1;i++){
		for(j = 0;j < ncolumn;j++){
			strcat(staff_info->msg,resultp[num++]);
			strcat(staff_info->msg,"      ");
		}
		strcat(staff_info->msg,"\n===========================================================\n");
	}
	send(acceptfd,staff_info,sizeof(T_StaffInfo),0);	
	sqlite3_close(db);
	return OK;
}

Type do_admin_modify(int acceptfd, PT_StaffInfo staff_info){
	sqlite3 *db;
    char *errmsg = NULL;
	char sql[128];
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}
    printf("age:%d\n",staff_info->age);
	if(strlen(staff_info->name) != 0){
		sprintf(sql,"update staff set name = '%s' where number = %d ",staff_info->name,staff_info->number);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
			fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
			return FALSE;	
		}
	}	
	if(strlen(staff_info->password) != 0){
		sprintf(sql,"update staff set password = '%s' where number = %d ",staff_info->password,staff_info->number);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
			fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
			return FALSE;	
		}
	} 
	if(staff_info->age != 0){
		sprintf(sql,"update staff set age = %d where number = %d ",staff_info->age,staff_info->number);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
			fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
			return FALSE;	
		}
	}
	if(staff_info->salary!=0){
		sprintf(sql,"update staff set salary = %d where number = %d ",staff_info->salary,staff_info->number);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
			fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
			return FALSE;	
		}
	}
	strcpy(staff_info->msg,"modify successful");
    send(acceptfd,staff_info,sizeof(T_StaffInfo),0);
	sqlite3_close(db);
}

Type do_admin_add(int acceptfd, PT_StaffInfo staff_info){
	sqlite3 *db;
    char *errmsg = NULL;
	char sql[256];    
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}
	sprintf(sql,"insert into staff values(%d,'%s',%d,'%s',%d,%d)",staff_info->number,staff_info->name,staff_info->type,staff_info->password,staff_info->age,staff_info->salary);
	
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
		fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
		return FALSE;	
	}
	strcpy(staff_info->msg,"Add OK");
    send(acceptfd,staff_info,sizeof(T_StaffInfo),0);
    sqlite3_close(db);
	return OK;
}

Type do_admin_del(int acceptfd, PT_StaffInfo staff_info){
	sqlite3 *db;
    char *errmsg = NULL;
	char sql[256]; 
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}
    printf("number:%d\n",staff_info->number);
	sprintf(sql,"delete from staff where number=%d ",staff_info->number);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
		fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
		return FALSE;	
	}
	strcpy(staff_info->msg,"delete successful");
    send(acceptfd,staff_info,sizeof(T_StaffInfo),0);
	sqlite3_close(db);
	return OK;
}

int history_callback(void* arg,int f_num,char** f_value,char** f_name)
{
	int acceptfd;
	T_StaffInfo buf;
	acceptfd = *((int *)arg);
	sprintf(buf.msg, "%s , %s", f_value[1], f_value[2]);
	send(acceptfd, &buf, sizeof(buf), 0);
	return 0;
}

Type do_admin_history(int acceptfd,  PT_StaffInfo       staff_info){
	char sql[128] = {0};
	char *errmsg;
	sqlite3 *db;   
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}
	sprintf(sql, "select * from staff");
	//查询数据库
	if(sqlite3_exec(db, sql, history_callback,(void *)&acceptfd, &errmsg) != 0)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Query record done.\n");
	}
	// 所有的记录查询发送完毕之后，给客户端发出一个结束信息
	staff_info->msg[0] = '\0';
	send(acceptfd, staff_info, sizeof(T_StaffInfo), 0);
	return OK;
}

Type do_user_query(int acceptfd, PT_StaffInfo  staff_info){
	sqlite3 *db;
    char **resultp;
    char *errmsg = NULL;
    int nrow = 0;
	int ncolumn = 0;
	char sql[128];
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}	
	sprintf(sql,"select * from staff where name='%s';",staff_info->name);
    printf("%s\n",sql);
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncolumn,&errmsg)!=0){	
		fprintf(stderr,"%s\n",errmsg);
		return FALSE;
	}	
    if(nrow == 1){
		//strcpy(staff_info->msg,"query successfulelly");
		//send(acceptfd,&login_info,sizeof(login_info),0);		
		int i,j,num = 0;		
			for(i = 0;i < nrow+1;i++)
			{
				for(j = 0;j < ncolumn;j++)
				{
					strcat(staff_info->msg,resultp[num++]);
					strcat(staff_info->msg,"      ");
				}
				strcat(staff_info->msg,"\n=================================================================\n");
			}
		send(acceptfd,staff_info,sizeof(T_StaffInfo),0);
		sqlite3_close(db);
        return OK;
	}
	else{
		sqlite3_close(db);
		return FALSE;
	}	
	return OK;
}

Type do_user_modify(int acceptfd, PT_StaffInfo staff_info){	
    sqlite3 *db;
    char *errmsg = NULL;
	char sql[128];
	if(sqlite3_open("StaffManage.db",&db) != 0){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return FALSE;
	}	
	if(strlen(staff_info->password) != 0){
		sprintf(sql,"update staff set password = '%s' where name = '%s' ",staff_info->password,staff_info->name);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != 0){
			fprintf(stderr,"sqlite3_exec failed :%s\n",errmsg);
			return FALSE;	
		}
	}
	strcpy(staff_info->msg,"modify successful");
    send(acceptfd,staff_info,sizeof(T_StaffInfo),0);
	sqlite3_close(db);
	return OK;
}
int main(int argc, const char *argv[])
{
	if(argc != 3)
	{
		printf("Usage:%s <server's ip> <server's port number>\n",argv[0]);
		return -1;
	}	
	int sockfd;
	int *p_acceptfd;
	if(database_init() == FALSE)
	{
		printf("create_database failed...\n");
		return -1;
	}
	/* 创建套接字 */
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket");
		return -1;
	}
	printf("sockfd is OK...\n");

	/* 绑定服务器ip和端口号 */
	struct sockaddr_in serveraddr = {     /* 填充结构体  */
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr.s_addr = htonl(INADDR_ANY)
	}; 
	if(bind(sockfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1) /* 绑定 */
	{
		perror("bind");
		exit(1);
	}
	printf("bind is OK...\n");
	/* 监听套接字 */
	if(listen(sockfd,8) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("listen is OK...\n"); 
    pthread_t tid;
	struct sockaddr_in clientaddr;
	while(1)
	{
		p_acceptfd = (int *)malloc(sizeof(int));
		socklen_t client_length = sizeof(clientaddr);
		*p_acceptfd = accept(sockfd,(struct sockaddr*)&clientaddr,&client_length);
		if(*p_acceptfd == -1)
		{
			perror("accept");
			exit(1);
		}
		printf("accept sucessful...\n");
		printf("The client ip is %s\n",inet_ntoa(clientaddr.sin_addr));

		/* 创建线程 */
		if(pthread_create(&tid,NULL,handle,(void *)p_acceptfd) != 0)
		{
			perror("pthread_create");
			exit(1);
		}	
	}
	close(sockfd);
	return 0;
}

