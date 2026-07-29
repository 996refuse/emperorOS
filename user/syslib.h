int fork();
void exit(int status);
int wait(int pid, int *ws);
struct timespec32 {
	int tv_sec;
	int tv_nsec;
};
int nanosleep(const struct timespec32 *req, struct timespec32 *rem);
void _putchar(char);
