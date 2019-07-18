#include<stdio.h>
#include<sys/types.h>  
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include"common.h"

Type do_login(int sockfd){
    T_LogIn login;  /* 定义登录信息结构体变量            */	
    /* 打印信息     */
	int choice;
	printf("**********************************************************************\n");
	printf("************ 1.Administrator   2.user  3.quit ************************\n");
	printf("**********************************************************************\n");
	printf("please input your choice:");
	scanf ("%d",&choice);
    /* 功能选择     */
	switch(choice){
		case 1:
			    login.type = ADMIN;
			    break;
	    case 2:
			    login.type = USER;
		        break;
		case 3:
			    printf("the client is quiting...\n");
				close(sockfd);
			    exit(0);
	    default:
	    	    printf("DoLogIn:input invalid...\n");
				exit(1);
	}		
	if(do_admin_or_user_login(sockfd,login) == FALSE){
		printf("DoLogIn:DoAdminLogin failed...\n");
		return FALSE;
	}	
	return OK;
}

Type do_admin_or_user_login(int sockfd,T_LogIn login){
    /* 输入姓名        */
	printf("please input name: ");
    scanf("%s",login.name);

	/* 输入密码 */
	printf("please input code: ");
	scanf("%s",login.password);

	/* 向服务器发送信息 */
	if(send(sockfd,&login,sizeof(login),0) == -1)
	{   
		perror("DoAdminOrUserLogin:send");
		printf("------%s-----%d---\n",__func__,__LINE__);
		return FALSE;
	}
	/* 接收服务器信息        */
	if(recv(sockfd,&login,sizeof(login),0) == -1){
		perror("recv");
		exit(1);
	}
	printf("%s\n",login.msg);	
    if(login.type == ADMIN){
		do_admin_opration(sockfd);
	}
	else if(login.type == USER){
        do_user_opration(sockfd,&login); 
    }	
	return OK;
}

Type do_user_opration(int sockfd,PT_LogIn login){
	T_StaffInfo staff_info = {0};
    strcpy(staff_info.name,login->name);
    /* 打印信息     */
	int choice;
	printf("**********************************************************************\n");
	printf("******************** 1.query  2.modify  3. quit **********************\n");
	printf("**********************************************************************\n");
	printf("please input your choice:");
	scanf ("%d",&choice);		

    /* 功能选择     */
	switch(choice){
		case 1:
			    staff_info.request_type = USERQUERY;
			    do_user_query(sockfd,&staff_info);
			    break;
	    case 2:
			    staff_info.request_type = USERMODIFY;
			    do_user_modify(sockfd,&staff_info);
		        break;
		case 3:
			    printf("the client is quiting...\n");
				close(sockfd);
			    exit(0);
	    default:
	    	    printf("DoLogIn:input invalid...\n");
				exit(1);
	}
	return OK;
}

Type do_user_query(int sockfd,PT_StaffInfo staff_info){
    /* 向服务器发送信息 */
	if(send(sockfd,staff_info,sizeof(T_StaffInfo),0) == -1)
	{   
		perror("DoUserQuery:send");
		printf("------%s-----%d---\n",__func__,__LINE__);
		return FALSE;
	}
	/* 接收服务器信息        */
	if(recv(sockfd,staff_info,sizeof(T_StaffInfo),0) == -1){
		perror("recv");
		exit(1);
	}	
	printf("%s\n",staff_info->msg);
	return OK;
}

Type do_user_modify(int sockfd, PT_StaffInfo staff_info){
	int choice;
	printf("**********************************************************************\n");
	printf("************************ 1.password 2.quit ***************************\n");
	printf("**********************************************************************\n");
	printf("please input your choice:");
	scanf ("%d",&choice);		
    
    /* 功能选择     */
	switch(choice){
		case 1:
			    printf("input password:");
	            scanf("%s",staff_info->password);
				if(send(sockfd,staff_info,sizeof(T_StaffInfo),0) == -1)
				{   
					perror("DoUserModify:send");
					printf("------%s-----%d---\n",__func__,__LINE__);
					return FALSE;
				}
			    break;
		case 2:
			    printf("the client is quiting...\n");
				close(sockfd);
			    exit(0);
	    default:
	    	    printf("DoLogIn:input invalid...\n");
				exit(1);
	}	
	/* 接收服务器信息*/
	if(recv(sockfd,staff_info,sizeof(T_StaffInfo),0) == -1){
		perror("recv");
		exit(1);
	}	
	printf("%s\n",staff_info->msg);	
	return OK;
}

Type do_admin_opration(int sockfd){
    /* 打印信息     */
	int choice;
	printf("**********************************************************************\n");
	printf("******** 1.query  2.modify  3.add 4. delete 5.history 6. quit ********\n");
	printf("**********************************************************************\n");
	printf("please input your choice:");
	scanf ("%d",&choice);		

    /* 功能选择*/
	switch(choice){
		case 1:
			    do_admin_query(sockfd);
			    break;
	    case 2:
			    do_admin_modify(sockfd);
		        break;
		case 3:
			    do_admin_add(sockfd);
				break;
		case 4: 
			    do_admin_delete(sockfd);
				break;
		case 5:
			    do_history(sockfd);
				break;
		case 6:
			    printf("the client is quiting...\n");
				close(sockfd);
			    exit(0);
	    default:
	    	    printf("DoLogIn:input invalid...\n");
				exit(1);
	}
	return OK;
}

Type do_admin_query(int sockfd){
	T_StaffInfo staff = {0};
	/* 打印信息     */
	int choice;
	printf("**********************************************************************\n");
	printf("********************* 1.name  2.all  3.quit **************************\n");
	printf("**********************************************************************\n");
	printf("please input your choice:");
	scanf ("%d",&choice);		

    /* 功能选择     */
	switch(choice){
		case 1:
				staff.request_type = ADMINNAME;
			    admin_query_forname(sockfd,&staff);
			    break;
	    case 2: staff.request_type = QUERYALL;
		        admin_query_forall(sockfd,&staff);
		        break;
		case 3:
			    printf("the client is quiting...\n");
				close(sockfd);
			    exit(0);
	    default:
	    	    printf("DoLogIn:input invalid...\n");
				exit(1);
	}	
}

Type admin_query_forname(int sockfd,PT_StaffInfo pstaff){	
    printf("please input name: ");
    scanf("%s",pstaff->name);
    // 向服务器发送信息
	if(send(sockfd,pstaff,sizeof(T_StaffInfo),0) == -1)
	{   
		perror("AdminQueryForName:send");
		printf("------%s-----%d---\n",__func__,__LINE__);
		return FALSE;
	}
	//接收服务器信息
	if(recv(sockfd,pstaff,sizeof(T_StaffInfo),0) == -1){
		perror("recv");
		exit(1);
	}	
	printf("%s\n",pstaff->msg);
	return OK;
}

Type admin_query_forall(int sockfd,PT_StaffInfo pstaff){
    /* 向服务器发送信息 */
	if(send(sockfd,pstaff,sizeof(T_StaffInfo),0) == -1)
	{   
		perror("AdminQueryForName:send");
		printf("------%s-----%d---\n",__func__,__LINE__);
		return FALSE;
	}
	/* 接收服务器信息        */
	if(recv(sockfd,pstaff,sizeof(T_StaffInfo),0) == -1){
		perror("recv");
		exit(1);
	}	
	printf("%s\n",pstaff->msg);
	return OK;
}

Type do_admin_modify(int sockfd){
	T_StaffInfo staff = {0};
	int num;
    printf("please input the number you want to modify:");
	scanf("%d",&num);
    staff.number = num;
	staff.request_type = ADMINMODIFY;	
	int choice;
	printf("**********************************************************************\n");
	printf("************* 1.name 2.password 3.age 4.salary 5.quit ****************\n");
	printf("**********************************************************************\n");
	printf("please input your choice:");
	scanf ("%d",&choice);		  
    /* 功能选择     */
	switch(choice){
		case 1:
			    printf("input name:");
	            scanf("%s",staff.name);
				if(send(sockfd,&staff,sizeof(T_StaffInfo),0) == -1)
				{   
					perror("DoAdminModify:send");
					printf("------%s-----%d---\n",__func__,__LINE__);
					return FALSE;
				}
			    break;
	    case 2: 
				printf("input password:");
	            scanf("%s",staff.password);
				if(send(sockfd,&staff,sizeof(T_StaffInfo),0) == -1)
				{   
					perror("DoAdminModify:send");
					printf("------%s-----%d---\n",__func__,__LINE__);
					return FALSE;
				}
		        break;
		case 3: 
				printf("input age:");
	            scanf("%d",&staff.age);
				if(send(sockfd,&staff,sizeof(T_StaffInfo),0) == -1)
				{   
					perror("DoAdminModify:send");
					printf("------%s-----%d---\n",__func__,__LINE__);
					return FALSE;
				}
		        break;
		case 4:
				printf("input salary:");
	            scanf("%d",&staff.salary);
				if(send(sockfd,&staff,sizeof(T_StaffInfo),0) == -1)
				{   
					perror("DoAdminModify:send");
					printf("------%s-----%d---\n",__func__,__LINE__);
					return FALSE;
				}
		        break;
		case 5:
			    printf("the client is quiting...\n");
				close(sockfd);
			    exit(0);
	    default:
	    	    printf("DoLogIn:input invalid...\n");
				exit(1);
	}	
	/* 接收服务器信息        */
	if(recv(sockfd,&staff,sizeof(T_StaffInfo),0) == -1){
		perror("recv");
		exit(1);
	}
	printf("%s\n",staff.msg);
	return OK;
}

Type do_admin_add(int sockfd){
	T_StaffInfo staff = {0};
	staff.request_type = ADMINADD;
	printf("input number:");
	scanf("%d",&staff.number);

	printf("input name:");
	scanf("%s",staff.name);

	printf("input type:");
	scanf("%d",&staff.type);

	printf("input password:");
	scanf("%s",staff.password);

	printf("input age:");
	scanf("%d",&staff.age);

	printf("input salary:");
	scanf("%d",&staff.salary);
	if(send(sockfd,&staff,sizeof(staff),0) == -1){	
		perror("DoAdminAdd:send");
		printf("------%s-----%d---\n",__func__,__LINE__);
		return FALSE;
	}
	if(recv(sockfd,&staff,sizeof(staff),0) == -1){
		perror("recv");
		exit(1);
	}
	printf("%s\n",staff.msg);
	return OK;
}

Type do_admin_delete(int sockfd){
	T_StaffInfo staff = {0};
	staff.request_type = ADMINDEL;

	printf("input number:");
	scanf("%d",&staff.number);
	printf("input name:");
	scanf("%s",staff.name);
    if(send(sockfd,&staff,sizeof(staff),0) == -1){	
		perror("DoAdminAdd:send");
		printf("------%s-----%d---\n",__func__,__LINE__);
		return FALSE;
	}
	if(recv(sockfd,&staff,sizeof(staff),0) == -1){
		perror("recv");
		exit(1);
	}
	printf("%s\n",staff.msg);
	return OK;	
}

Type do_history(int sockfd){
	T_StaffInfo staff = {0};
	staff.request_type = ADMINHISTORY;	
	send(sockfd, &staff, sizeof(staff), 0);		
	// 接受服务器，传递回来的历史记录信息
	while(1)
	{
		recv(sockfd, &staff, sizeof(staff), 0);
		if(staff.msg[0] == '\0')
			break;
		//输出历史记录信息
		printf("%s\n", staff.msg);
	}
	return OK;
}

int main(int argc,const char *argv[]){
	if(argc != 3){
		printf("Usage:%s <server's ip> <server's port number>\n",argv[0]);
		return -1;
	}
	int sockfd;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	printf("sockfd is OK...\n");	
	/* 连接客户端*/
	struct sockaddr_in serveraddr = {     
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr.s_addr = inet_addr(argv[1])
	};    
	if(connect(sockfd, (const struct sockaddr*)&serveraddr,sizeof(serveraddr)) == -1){	/*连接*/
		perror("connect");
		exit(1);
	}
    if(do_login(sockfd) == FALSE){
		printf("main:DoLogIn failed...\n");
		return -1;
	}	
	close(sockfd);
	return 0;
}	

