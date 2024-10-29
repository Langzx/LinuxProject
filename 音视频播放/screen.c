#include<stdio.h>
#include<linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include"./include/screen.h"
#include <unistd.h>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>
struct fb_fix_screeninfo fb_fix;
struct fb_var_screeninfo fb_var;
extern int alltime;
struct _screen_info{
    unsigned int weith;
    unsigned int height;
    unsigned int line_size;
    unsigned int screen_size;
    unsigned int *screen_base;
}screen_info;
int screen_init()
{
    int fd = open("/dev/fb0",O_RDWR);
    if(0 > fd){
        perror("lcd open error");
        return -1;
    }
    ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);
    ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    screen_info.weith = fb_var.xres;
    screen_info.height = fb_var.yres;
    screen_info.line_size = fb_fix.line_length;
    screen_info.screen_size = screen_info.line_size*fb_var.yres;
    screen_info.screen_base = mmap(NULL, screen_info.screen_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(screen_info.screen_base == MAP_FAILED){
        perror("mmap error");
        close(fd);
        return -1;
    }
    return 0;
}
/* 指定功能区开始位置 */
int screen_func(int y)
{

    unsigned int s_x;
    for(s_x = 800*y; s_x < 800*480; s_x++){
        *(screen_info.screen_base+s_x) = 0x0;
    }
    return 0;
}
/* 进度条 */
int screen_process(char *x,int y)
{

    if(x == NULL){
        return -1;
    }
    char *endptr;
    unsigned int x_end;
    unsigned int s_x;
    unsigned int num = strtol(x,&endptr,10);
    if (*endptr == '\0' ) { 
        x_end = num*8;// 检查是否转换成功   
    }else {
      return -1;
    }
    unsigned int s_y = y;
     x_end += 800*s_y;
    
    for(s_y = y;s_y <480; s_y++){
        
        for(s_x = 800*s_y;s_x < x_end; s_x++){
            *(screen_info.screen_base+s_x) = 0xFF00;
        }
        x_end += 800;
    }
    //memcpy(screen_info.screen_base,buf,screen_info.screen_size); 效率会高，但会闪屏，
    return 0;
}
int hasEqualFollowedByDigits(const char *input) {
    int i ;
    // 遍历输入字符串的每个字符
    for (i = 0; input[i] != '\0'; ++i) {
        // 检查字符是否为等号
        if (input[i] == '=') {
            // 检查等号后面的字符是否为数字
            if (isdigit(input[i + 1])) {
                return 1; // 找到了'=数字'模式
            }
        }
    }
    return 0; // 没找到'=数字'模式
}

void extractDigitsAfterEqual(const char *input, char *output) {
    int outIndex = 0; // 用于记录输出字符串的位置
    int foundEqual = 0; // 标记是否找到了等号
    int collectingDigits = 0; // 标记是否开始收集数字
    int i;
    // 遍历输入字符串的每个字符
    for (i = 0; input[i] != '\0'; ++i) {
        // 检查字符是否为等号
        if (input[i] == '=') {
            foundEqual = 1;
            collectingDigits = 1; // 等号后开始收集数字
        } else if (collectingDigits && isdigit(input[i])) {
            output[outIndex++] = input[i]; // 收集数字
        } else if (foundEqual && !isdigit(input[i])) {
            collectingDigits = 0; // 停止收集数字
        }
    }

    // 在字符串末尾添加终止符
    output[outIndex] = '\0';
}
int command(char *data,int fifo_fd)
{ 
    char command[1024];
    sprintf(command, "echo %s > video_fifo", data);   
    system(command);

    return 0;
}
/* int getAllTime(int *pipe_fd)
{
    int time;
    read(pipe_fd[0],&time,sizeof(time));
    return time;
} */
/* 获取当前进度时间 */
char *getCurrentTime(char *data,int *pipe_fd)
{
    char updata[1024];
    bzero(data, 1024);
    read(pipe_fd[0],data,sizeof(data));
    if (hasEqualFollowedByDigits(data)) {
        extractDigitsAfterEqual(data, updata);
        strcpy(data,updata);
    }else{
        data = NULL;
    }
    return data;
}

