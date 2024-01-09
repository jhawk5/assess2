struct stat;
struct rtcdate;
struct rect;
struct GraphicsOperations;


// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int getch(void);
int setvideomode(int);
int setpixel(int hdc, int x, int y);
int lineto(int hdc, int x, int y);
int moveto(int hdc, int x, int y);
int selectpen(int hdc, int index);
int setpencolour(int index, int r, int g, int b);
int fillrect(int, struct rect*);
int beginpaint(int hwnd);
int endpaint(int hdc, struct GraphicsOperations*, int operationcount);
void clear640x400x16();



// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
