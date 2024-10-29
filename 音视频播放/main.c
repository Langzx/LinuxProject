#include"./include/screen.h"
#include"./include/touch.h"
#include"./include/video.h"
#include<pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <unistd.h>
#include <stdio.h>
#include<string.h>
#include <sys/wait.h>
#define FUNCLOCAL_Y 380
#define VIDEOPATH "/home/video"
char data[1024];
int fifo_fd;
int fd_pipe[2]; //创建无名管道，用于从mplayer读取命令
_Bool my_signal = 0;
pthread_cond_t  cond;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int alltime = 0;
char path[256];
struct _video_info *video_info;
/* 该线程用于向mplayer发送获取时间指令 */
void *getTime(void *arg)
{  
    char *send;
    send = (char *)arg;
    while(1){
        pthread_mutex_lock(&mutex1);
        /* 当暂停时，停止获取时间指令，因为发送时间指令会开启播放 */
        while(my_signal == 1){
             pthread_cond_wait(&cond, &mutex1);     //等待唤醒
        }
        command(send, fifo_fd); 
        pthread_mutex_unlock(&mutex1);
    }

   return 0;
}
void *send_pause(void *arg)
{
    while(1){
        pthread_mutex_lock(&mutex2);
        if(video_pause()==1){
            /* 每次按下屏幕改变状态 0暂停 1恢复 */
            my_signal = !my_signal;
            if(my_signal == 0){
                pthread_cond_signal(&cond);     //当恢复时，唤醒发生时间指令线程
            }
            char *send;
            send = (char *)arg;   
            command(send, fifo_fd);
        }
        pthread_mutex_unlock(&mutex2);
    }
   
   return 0;
}
void *send_process(void *arg)
{
    int num;
    char send[1024];
    while(1){
        num = screen_touch()/8;
        if(num < 0){
            screen_func(FUNCLOCAL_Y);
        }
        sprintf(send,"seek %d",num);
        command(send,fifo_fd);
    }
}
void *video_quit(void *arg)
{
    char q[1];
    fgets(q, 2, stdin);
    if(strcmp(q,"q") == 0){
        command("quit",fifo_fd);        //后退时，重置进度条绘制
        
        exit(0);
    }
    return 0;
}
int main(int argc, char const *argv[])
{
    int ret;
    /* 判断管道文件是否存在 */
    /* 发送指令到管道，mplayer通过管道回复信息，通过标准输出写入无名管道，再从无名管道内读出信息 */
    if(access("video_fifo", F_OK)){
        ret = mkfifo("video_fifo", O_CREAT|0644);
        if(0 > ret){
            perror("mkfifo error");
            return -1;
        }
    }
    fifo_fd = open("./video_fifo", O_RDWR);     //打开有名管道
    if(0 > fifo_fd){
        perror("fifo_fd open error");
        return -1;
    }
    if(pipe(fd_pipe) < 0) //创建无名管道
	{
		perror("pipe create error");
		return -1;
	}
    video_info = calloc(1,sizeof(struct _video_info));
    findVideo_init(VIDEOPATH,video_info);
    int list_fd = open("./list.txt",O_RDWR);
    getVideoFileName(video_info,path);
    ret = write(list_fd,path,sizeof(path));
    if(0 > ret){
        perror("write error");
        return -1;
    }
    pthread_cond_init(&cond, NULL);
    pid_t pid = fork();
    /* 父进程 */
    if(pid > 0){
        close(fd_pipe[1]);
        screen_init();
        screen_func(FUNCLOCAL_Y);
        pthread_t time;
        pthread_t pause;
        pthread_t process;
        pthread_t quit;
        /* command("get_time_length",fifo_fd);
        alltime = getAllTime(&fd_pipe[0]); */
        pthread_mutex_init(&mutex1,NULL);
        pthread_mutex_init(&mutex2,NULL);
        pthread_create(&time,NULL,getTime,"get_percent_pos");  
        pthread_create(&pause,NULL,send_pause,"pause");
        pthread_create(&process,NULL,send_process,NULL);     
        pthread_create(&quit,NULL,video_quit,NULL);
        while(1){
            screen_process(getCurrentTime(data,&fd_pipe[0]),FUNCLOCAL_Y);       //显示当前进度

        }
      
            
    }
    /* 子进程 */
    else if(pid == 0){
        close(fd_pipe[0]);
		dup2(fd_pipe[1], 1); //将子进程的标准输出重定向到管道的写端(不知道为什么，向管道发送命令后，得到的回复不在管道文件中，在标准输出)
        
      	execlp("mplayer", "mplayer", "-slave", "-quiet", "-input", "file=./video_fifo",getVideoFileName(video_info,path), NULL);
        exit(0);
    }  

    return 0;
}
