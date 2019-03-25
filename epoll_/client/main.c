/*************************************************************************
    > File Name: main.c
    > Author: malunkun
    > Mail: malunkun<209446860@qq.com>
    > Created Time: 2019年03月25日 星期一 16时20分00秒
 ************************************************************************/
#include"client.h"
int main(int argc,char *argv[])
{
	char log_buf[128];
	int no_running = 0;
	int no_connect = 0;
	int no_get_temp= 0;
	int port=0;
	int ret = -1;
	FILE *fp=NULL;
	char ip[1024];
	char send_buf[1024];
	time_t timep;
	int con_fd = 0;
	float temp = 0.00;
	time (&timep);//获取时间
	memset(log_buf,0,sizeof(log_buf));
	fp = my_syslog(log_buf);
	if(NULL == fp)
	{
		printf("创建日志文件失败！\n");
		return -1;
	}
	daemon(0,1);  //守护进程
	memset(ip,0,sizeof(ip));
	ret = get_parameter(argc,argv,&port,ip);
	if(ret<0)
	{
		printf("参数不完整\n");
		return -1;
	}
	while(!no_running)
	{
		while(!no_connect)
		{
			con_fd = connect_server(ip,port,fp);
			if(con_fd<0)
			{
				printf("连接服务器失败!\n");
				return -2;
			}
			no_connect = 1;
			fprintf(fp,"成功连接%s,端口:%d\n",ip,port);
		}
		while(!no_get_temp)
		{
			if (get_temper(&temp,fp) < 0)
				{ 
					fprintf(fp, "温度获取失败:%s\n",strerror(errno));
					no_get_temp = 0;
				}
				fprintf(fp,"获取到温度:%f\n",temp);
				memset(send_buf,0,sizeof(send_buf));
				sprintf(send_buf,"%s|%.3f°C|%s",ID,temp,asctime(gmtime(&timep)));
				fprintf(fp,"发送内容:%s\n",send_buf);
				if(write(con_fd,&send_buf,sizeof(send_buf)) <0)
				{
					fprintf(fp,"w rite fail:%s\n",strerror(errno));
					no_connect = 0;
					break;
				}
				if(read(con_fd,&send_buf,sizeof(send_buf)) <= 0)
				{
					fprintf(fp,"read fail:%s\n",strerror(errno));
					no_connect = 0;
					break;
				}
				sleep(5);
		}
	}
	close(con_fd);
	my_syslog_close(fp);
	return 0;
}
