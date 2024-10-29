#include <stdio.h>
#include <tslib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdlib.h>
#include <pthread.h>
#include"./include/screen.h"
#define TS_PATH "/dev/input/event1"


int screen_touch()
{
    int x,y,x_next,y_next;
    struct tsdev *ts = NULL;
    struct ts_sample sample = {0};
   int ret;
   int pressure;        //用于控制是否拖动
   int distance;
    ts = ts_setup(NULL,0);
    if(ts == NULL){
        perror("ts_setup error");
        return -1;
    }
    while(1){
        if(0 > ts_read(ts,&sample,1)){
            perror("ts_read error");
            return -1;
        }
        /* tslib只有在按下或释放时才会触发pressure,也就是说，就算不按下，else也不会一直触发 */
        /* 如果在指定的功能区范围内拖动 */
        if(sample.pressure && sample.y > 380){    
            if(pressure == 1){
                /* 拖动一直更新的坐标 */
                x_next = sample.x;
                //y_next = sample.y;
            }else{
                /* 保存第一次按下的起始坐标 */
                pressure = 1;
                x = sample.x;               
                //y = sample.y;
            }
        }else{
            
            if(pressure == 1){
                distance = x_next - x;
                pressure  = 0;
                return distance;
            }
            pressure = 0;
            x_next = 0;
            y_next = 0;
        }
    }  
}
int video_pause()
{
    struct tsdev *ts = NULL;
    struct ts_sample sample = {0};
    int y;
    _Bool pressure = 0;        //用于控制是否拖动
    ts = ts_setup(NULL,0);
    if(ts == NULL){
        perror("ts_setup error");
        return -1;
    }
    while(1){
        if(0 > ts_read(ts,&sample,1)){
            perror("ts_read error");
            return -1;
        }
        /* tslib只有在按下或释放时才会触发pressure,也就是说，就算不按下，else也不会一直触发 */
        if(sample.pressure){    
            y = sample.y;
                
        }else{
            /* 如果在指定位置按下 */
            if(y < 380){
                pressure = 1;
                return pressure;  
            }
          
        }
    }
   return 0;
}
