struct _video_info{
    char file_name[256];
    char file_path[256];
    struct _video_info *next;
    struct _video_info *prep;
    struct _video_info *head;
};
int findVideo_init(char *pathname,struct _video_info *video_info);
char *getVideoFileName(struct _video_info *video_info,char *path);