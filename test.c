#include "types.h"
#include "graphics.h"
#include "user.h"

#define set_pixel 1
#define move_to 2
#define line_to 3
#define set_pen_colour 4
#define select_pen 5

#define maxoperations 500

struct GraphicsOperations operations[maxoperations];
int operationcount = 0;



int buffered_setpixel(int hdc, int x, int y){
    operations[operationcount] = (struct GraphicsOperations){set_pixel, hdc, x, y};
    operationcount++;
    return 0;
}

int buffered_moveto(int hdc, int x, int y){
    operations[operationcount] = (struct GraphicsOperations){move_to, hdc, x, y};
    operationcount++;
    return 0;
}

int buffered_lineto(int hdc, int x, int y){
    operations[operationcount] = (struct GraphicsOperations){line_to, hdc, x, y};
    operationcount++;
    return 0;
}

int buffered_setpencolour(int index, int r, int g, int b){
    operations[operationcount].index = set_pen_colour;
    operations[operationcount].penindex = index;
    operations[operationcount].r = r;
    operations[operationcount].g = g;
    operations[operationcount].b = b;
    operationcount++;
    return 0;
}

int buffered_selectpen(int hdc, int index){
    operations[operationcount].index = select_pen;
    operations[operationcount].penindex = index;
    operations[operationcount].hdc = hdc;
    operationcount++;
    return 0;
}

struct rect rectangle = {10, 10, 300, 500};

int main(int argc, char *argv[]) {
    
    setvideomode(0x13);
    int hdc = beginpaint(0);
    for (int i = 0; i < 20; i++)
    {
        buffered_moveto(hdc, i * 10, i * 5);
        buffered_setpencolour(i + 20, i * 3, i * 2, i);
        buffered_selectpen(hdc, i + 20);
        buffered_lineto(hdc, i * 10 + 20, i * 5);
        buffered_lineto(hdc, i * 10 + 20, i * 5 + 20);
        buffered_lineto(hdc, i * 10, i * 5 + 20);
        buffered_lineto(hdc, i * 10, i * 5);
    }
    endpaint(hdc, operations, operationcount);
    getch();
    setvideomode(0x12);
    selectpen(0, 1);
    fillrect(0, &rectangle);
    getch();
    setvideomode(0x03);

    exit();


}