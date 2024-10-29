int screen_init();
int screen_func(int y);
char *getCurrentTime(char *data,int *pipe_fd);
int command(char *data,int fifo_fd);
int screen_process(char *x,int y);
int hasEqualFollowedByDigits(const char *input);
void extractDigitsAfterEqual(const char *input, char *output);
//int getAllTime(int *pipe_fd);