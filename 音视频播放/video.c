#include<stdio.h>
#include <stdlib.h>
 #include <unistd.h>
 #include<pthread.h>
 #include <dirent.h>
#include <sys/stat.h>
#include<string.h>
#include"./include/video.h"

int findVideo_init(char *pathname,struct _video_info *video_info)
{
    char name[256];
    struct stat dir_stat;
    struct dirent *dirent_info;
    DIR *dp;
    dp = opendir(pathname);
    if(dp == NULL){
        perror("open dir error");
        return -1;
    }
    dirent_info = readdir(dp);
    if(dirent_info == NULL){
        perror("read dir error");
        return -1;
    }
    int ret = stat(pathname,&dir_stat);
    if(0 > ret){
        perror("stat error");
        return -1;
    }
    while(dirent_info){
        if(strncmp(dirent_info->d_name, ".", 1 ) != 0){
            if(dirent_info->d_type == 8){
                if(strstr(dirent_info->d_name,".avi")){
                    struct _video_info *p = calloc(1,sizeof(struct _video_info));
                    strcpy(p->file_name,dirent_info->d_name);
                    strcpy(p->file_path,pathname);
                    if(video_info->head == NULL){
                        video_info->head = p;
                        p->prep = p;
                        p->next = NULL;
                    }
                    
                    else{
                        struct _video_info *q = video_info;
                        while(q->next){
                            q = q->next;
                        }
                        p->head = video_info->head;
                        q->next = p;
                        p->prep = q;
                        p->next = NULL;
                    }
                }
            }
            if(dirent_info->d_type == 4){
                sprintf(name, "%s/%s", pathname,dirent_info->d_name);
                findVideo_init(name,video_info);
            }
        }
        dirent_info = readdir(dp);
    }
    return 0;
}
char *getVideoFileName(struct _video_info *video_info,char *path)
{
    struct _video_info *p = video_info;
    while(p){
        sprintf(path,"%s/%s",p->file_path,p->file_name);
        p = p->next;
    }
    return path;
}

