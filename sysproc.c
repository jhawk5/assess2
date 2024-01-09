#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

struct rect {
 int top; // y co-ordinate of top of rectangle
 int left; // x co-ordinate of left of rectangle
 int bottom; // y co-ordinate of bottom of rectangle
 int right; // x co-ordinate of right of rectangle
};

struct GraphicsOperations {
    int index;
    int hdc;
    int x;
    int y;
    int penindex;
    int r;
    int g;
    int b;
};

int sys_fork(void) {
    return fork();
}

int sys_exit(void) {
    exit();
    return 0;  // not reached
}

int sys_wait(void) {
    return wait();
}

int sys_kill(void) {
    int pid;

    if (argint(0, &pid) < 0) {
        return -1;
    }
    return kill(pid);
}

int sys_getpid(void) {
    return myproc()->pid;
}

int sys_sbrk(void) {
    int addr;
    int n;

    if (argint(0, &n) < 0) {
        return -1;
    }
    addr = myproc()->sz;
    if (growproc(n) < 0) {
        return -1;
    }
    return addr;
}

int sys_sleep(void) {
    int n;
    uint ticks0;

    if (argint(0, &n) < 0) {
        return -1;
    }
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

int sys_setpixel(void) {
    int hdc, x, y;
    if (argint(0, &hdc) < 0 || argint(1, &x) < 0 || argint(2, &y) < 0)
        return -1; // Return -1 if unable to retrieve arguments

    return setpixel(hdc, x, y); // Assuming hdc parameter is not used in this stage
}

int sys_moveto(void) {
    int hdc, x, y;
    if (argint(0, &hdc) < 0 || argint(1, &x) < 0 || argint(2, &y) < 0)
        return -1; // Return -1 if unable to retrieve arguments

    return moveto(hdc, x, y); // Assuming hdc parameter is not used in this stage
}

int sys_lineto(void) {
    int hdc, x, y;
    if (argint(0, &hdc) < 0 || argint(1, &x) < 0 || argint(2, &y) < 0)
        return -1; // Return -1 if unable to retrieve arguments

    return lineto(hdc, x, y); // Assuming hdc parameter is not used in this stage
}

int sys_clear320x200x256(void) {
    clear320x200x256();
    return 0;
}

int sys_setpencolour(void){
    int index, r, g, b;
    if (argint(0, &index) < 0 || argint(1, &r) < 0 || argint(2, &g) < 0 || argint(3, &b) < 0)
        return -1; // Return -1 if unable to retrieve arguments
    return setpencolour(index, r, g, b);
}

int sys_selectpen(void){
    int hdc, index;
    if (argint(0, &hdc) < 0 || argint(1, &index) < 0)
        return -1; // Return -1 if unable to retrieve arguments
    return selectpen(hdc, index);
}

int sys_fillrect(void){
    struct rect *rectangle;
    int hdc;
    if (argint(0, &hdc) < 0)
        return -1;
    if (argptr(1, (void *)&rectangle, sizeof(*rectangle)) < 0)
        return -1;
    return fillrect(hdc, rectangle);
}

int sys_beginpaint(void){
    int hwnd;
    if (argint(0, &hwnd) != 0)
        return -1;
    return beginpaint(hwnd);
}

int sys_endpaint(void){
    int hdc, operationcount;
    struct GraphicsOperations *operations;
    if (argint(0, &hdc) < 0)
        return -1;
    if (argptr(1, (void *)&operations, sizeof(*operations)) < 0)
        return -1;
    if (argint(2, &operationcount) < 0)
        return -1;
    return endpaint(hdc, operations, operationcount);
}

int sys_clear640x400x16(void){
    clear640x400x16();
    return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void) {
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}
