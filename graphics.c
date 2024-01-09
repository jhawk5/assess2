#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "x86.h"
#include "graphics.h"

#define set_pixel 1
#define move_to 2
#define line_to 3
#define set_pen_colour 4
#define select_pen 5

#define maxcontexts 50 

struct planes black = {0, 0, 0, 0};
struct planes blue = {1, 0, 0, 0};
struct planes green = {0, 1, 0, 0};
struct planes cyan = {1, 1, 0, 0};
struct planes red = {0, 0, 1, 0};
struct planes magenta = {1, 0, 1, 0};
struct planes brown = {0, 1, 1, 0};
struct planes light_gray = {1, 1, 1, 0};
struct planes dark_gray = {0, 0, 0, 1};
struct planes light_blue = {1, 0, 0, 1};
struct planes light_green = {0, 1, 0, 1};
struct planes light_cyan = {1, 1, 0, 1};
struct planes light_red = {0, 0, 1, 1};
struct planes light_magenta = {1, 0, 1, 1};
struct planes yellow = {0, 1, 1, 1};
struct planes white = {1, 1, 1, 1};
struct planes currentColour = {1, 1, 1, 1};

struct GraphicsContext contexts[maxcontexts];
int nexthdc = 0;

void clear640x400x16() {
    // Since each plane holds 640x400 pixels, clear each plane separately
    for (int plane = 0; plane < 4; ++plane) {
        // Select the current plane
        setplane(plane);

        uchar *videoBufferBase = getframebufferBase(); // Adjust pointer to starting point of the plane
        memset(videoBufferBase, 0, 640 * 400); // Set all pixels to black (color index 0)
    }
}



void clear320x200x256() {
	// You need to put code to clear the video buffer here.  Initially, 
	// you might just set each pixel to black in a nested loop, but think
	// about faster ways to do it. 
	//
	// This function is called from videosetmode.
    ushort *videoBuffer = (ushort *)P2V(0xA0000);
    // Fill the entire video buffer with black (color index 0)
    memset(videoBuffer, 0, 320 * 200); // Set all pixels to black (color index 0)
    
}

int setpixel(int hdc, int x, int y) {
    uchar currentVideoMode = getcurrentvideomode();
    if (currentVideoMode == 0x13){
        int physicaladdress = 0xA0000 + 320 * y + x;

        unsigned char *pointer = (unsigned char *)P2V(physicaladdress); // Convert physical address to kernel virtual address
        //cprintf("Address: %p\n", ptr); // Print the memory address for debugging
        *pointer = contexts[hdc].selectedPen; // Write the byte to the specified physical address
    }
    else if (currentVideoMode == 0x12){
        for (int plane = 0; plane < 4; ++plane) {
            // Select the current plane
            setplane(plane);

            uchar *videoBufferBase = getframebufferBase(); // Adjust pointer to starting point of the plane
            uchar *pointer = videoBufferBase + (640 * y + x) / 8;
            switch(plane) {
                case 0:
                    *pointer = currentColour.plane0; // Write the byte to the specified physical address
                    break;
                case 1:
                    *pointer = currentColour.plane1; // Write the byte to the specified physical address
                    break;
                case 2:
                    *pointer = currentColour.plane2; // Write the byte to the specified physical address
                    break;
                case 3:
                    *pointer = currentColour.plane3; // Write the byte to the specified physical address
                    break;
                default:
                    break;
            }
            
        }
    }
	return 0;

}

// Function to update current graphics position
int moveto(int hdc, int x, int y) {
    uchar currentVideoMode = getcurrentvideomode();
    if (currentVideoMode == 0x13){
        if (x < 1) x = 1; // Clip x coordinate if it's negative
        if (x >= 320) x = 319; // Clip x coordinate to screen width - 1
        if (y < 1) y = 1; // Clip y coordinate if it's negative
        if (y >= 200) y = 199; // Clip y coordinate to screen height - 1
    }
    else if (currentVideoMode == 0x12){
        if (x < 1) x = 1; // Clip x coordinate if it's negative
        if (x >= 640) x = 639; // Clip x coordinate to screen width - 1
        if (y < 1) y = 1; // Clip y coordinate if it's negative
        if (y >= 400) y = 399; // Clip y coordinate to screen height - 1
    }

    contexts[hdc].currentX = x;
    contexts[hdc].currentY = y;

    return 0;
}

// Function to draw a line from current position to (x, y)
int lineto(int hdc, int x, int y) {
    uchar currentVideoMode = getcurrentvideomode();
    if (currentVideoMode == 0x13){
        if (x < 1) x = 1; // Clip x coordinate if it's negative
        if (x >= 320) x = 319; // Clip x coordinate to screen width - 1
        if (y < 1) y = 1; // Clip y coordinate if it's negative
        if (y >= 200) y = 199; // Clip y coordinate to screen height - 1
    }
    else if (currentVideoMode == 0x12){
        if (x < 1) x = 1; // Clip x coordinate if it's negative
        if (x >= 640) x = 639; // Clip x coordinate to screen width - 1
        if (y < 1) y = 1; // Clip y coordinate if it's negative
        if (y >= 400) y = 399; // Clip y coordinate to screen height - 1
    }
    
    int dx = x - contexts[hdc].currentX;
    int dy = y - contexts[hdc].currentY;
    int sx = 1, sy = 1;

    // Implementing Bresenham's line drawing algorithm
    if (dx < 0) {
        sx = -1;
        dx = -dx;
    }
    if (dy < 0){
        sy = -1;
        dy = -dy;
    }
    
    int err, e2;
    if (dx > dy){
        err = dx / 2;
    } else {
        err = -dy / 2;
    }

    while (contexts[hdc].currentX != x || contexts[hdc].currentY != y) {
        // Call setpixel function to draw the pixel at currentPos.x and currentPos.y
        //cprintf("x: %p\n", currentPos.x);
        //cprintf("y: %p\n", currentPos.y);
        setpixel(hdc, contexts[hdc].currentX, contexts[hdc].currentY);

        e2 = 2 * err;
        if (e2 > -dx) {
            err -= dy;
            contexts[hdc].currentX += sx;
            if (contexts[hdc].currentX == 0){
                contexts[hdc].currentX = 1;
            }
        }
        if (e2 < dy) {
            err += dx;
            contexts[hdc].currentY += sy;
            if (contexts[hdc].currentY == 0){
                contexts[hdc].currentY = 1;
            }
        }
    }

    return 0;
}

int setpencolour(int index, int r, int g, int b){
    uchar currentVideoMode = getcurrentvideomode();
    if (index < 16 || index > 255 || r < 0 || r > 63 || g < 0 || g > 63 || b < 0 || b > 63 || currentVideoMode != 0x13) {
        return -1; // Invalid values supplied
    }


    outb(0x3C8, index);
    outb(0x3C9, r);
    outb(0x3C9, g);
    outb(0x3C9, b);

    return 0;
}

int selectpen(int hdc, int index){
    uchar currentVideoMode = getcurrentvideomode();
    if (currentVideoMode == 0x13){
        contexts[hdc].selectedPen = index;
    }
    else if (currentVideoMode == 0x12){
        switch (index){
            case 0:
                currentColour = black;
                break;
            case 1:
                currentColour = blue;
                break;
            case 2:
                currentColour = green;
                break;
            case 3:
                currentColour = cyan;
                break;
            case 4:
                currentColour = red;
                break;
            case 5:
                currentColour = magenta;
                break;
            case 6:
                currentColour = brown;
                break;
            case 7:
                currentColour = light_gray;
                break;
            case 8:
                currentColour = dark_gray;
                break;
            case 9:
                currentColour = light_blue;
                break;
            case 10:
                currentColour = light_green;
                break;
            case 11:
                currentColour = light_cyan;
                break;
            case 12:
                currentColour = light_red;
                break;
            case 13:
                currentColour = light_magenta;
                break;
            case 14:
                currentColour = yellow;
                break;
            case 15:
                currentColour = white;
                break;
            default:
                return -1;
        }
    }

    return 0;
}

int fillrect(int hdc, struct rect * rect){
    uchar currentVideoMode = getcurrentvideomode();
    if (currentVideoMode == 0x13){
        if (rect->left < 1) rect->left = 1; // Clip x coordinate if it's negative
        if (rect->right >= 320) rect->right = 319; // Clip x coordinate to screen width - 1
        if (rect->top < 1) rect->top = 1; // Clip y coordinate if it's negative
        if (rect->bottom >= 200) rect->bottom = 199; // Clip y coordinate to screen height - 1
    }
    else if (currentVideoMode == 0x12){
        if (rect->left < 1) rect->left = 1; // Clip x coordinate if it's negative
        if (rect->right >= 640) rect->right = 639; // Clip x coordinate to screen width - 1
        if (rect->top < 1) rect->top = 1; // Clip y coordinate if it's negative
        if (rect->bottom >= 400) rect->bottom = 399; // Clip y coordinate to screen height - 1
    }

    int x, y;
    // Iterate through the rectangle's coordinates
    for (y = rect->top; y <= rect->bottom; y++) {
        for (x = rect->left; x <= rect->right; x++) {
            // Set the pixel at coordinates (x, y) to the current pen color
            setpixel(hdc, x, y);
        }
    }

    return 0;
}

int beginpaint(int hwnd){
    if (nexthdc > maxcontexts){
        return -1;
    }

    struct GraphicsContext CurrentContext;
    CurrentContext.currentX = 0;
    CurrentContext.currentY = 0;
    CurrentContext.selectedPen = 15;

    contexts[nexthdc] = CurrentContext;
    nexthdc++;


    return (nexthdc-1);
}

int endpaint(int hdc, struct GraphicsOperations * operations, int operationcount){
    if (hdc < 0 || hdc > maxcontexts){
        return -1;
    }
    for (int i = 0; i < operationcount; ++i) {
        struct GraphicsOperations op = operations[i];
        switch (op.index) {
            case move_to:
                moveto(op.hdc, op.x, op.y);
                break;
            case line_to:
                lineto(op.hdc, op.x, op.y);
                break;
            case set_pixel:
                setpixel(op.hdc, op.x, op.y);
                break;
            case set_pen_colour:
                setpencolour(op.penindex, op.r, op.g, op.b);
                break;
            case select_pen:
                selectpen(op.hdc, op.penindex);
                break;
            default:
                return -1;
                break;
        }
    }
    
    // Reset operation buffer
    operationcount = 0;
    contexts[hdc].currentX = 0;
    contexts[hdc].currentY = 0;
    contexts[hdc].selectedPen = 0;
    return 0;
}
