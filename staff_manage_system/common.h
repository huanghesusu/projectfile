#ifndef _COMMOM_H_
#define _COMMOM_H_


/* 宏定义函数返回状态   		  */
typedef int Type;  
#define OK    1
#define FALSE 0

/* 宏定义一些指令        */
#define  ADMIN              0            /* 管理员登录	     */
#define  USER               1            /* 普通用户登录       */
#define  ADMINNAME          2            /* 管理员按名字查找 */
#define  QUERYALL           3            /* 查找所有     */
#define  ADMINMODIFY        4            /* 管理员修改信息  */
#define  ADMINADD           5            /* 管理员添加员工  */
#define  ADMINDEL           6            /* 管理员删除员工 */
#define  ADMINHISTORY       7            /* 查询历史记录 */
#define  USERQUERY          8            /* 普通用户查询  */
#define  USERMODIFY         9            /* 普通用户修改   */

/* 声明员工信息结构体 */
typedef struct StaffInfo{
	int  number;             /* 员工工号 */
	int  age;                /* 员工年龄 */
	int  salary;             /* 员工工资 */
	int  type;               /* 员工类型：管理员or普通用户*/
	int  request_type;       /* 客户端请求操作类型*/
	char name[12];           /* 员工姓名 */
	char password[8];        /* 员工登录密码*/
	char msg[1024];          /* 通信信息  */
}T_StaffInfo,*PT_StaffInfo;

/*  声明客户端和服务器通讯结构体           	  */
typedef struct LogIn{
	char name[12];           /* 登录用户名*/
	char password[12];       /* 登录密码  */
	char msg[64];            /* 通讯信息  */
	int  type;               /* 登录选择  */
	
}T_LogIn,*PT_LogIn;

#endif

















