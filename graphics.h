

struct rect {
 int top; // y co-ordinate of top of rectangle
 int left; // x co-ordinate of left of rectangle
 int bottom; // y co-ordinate of bottom of rectangle
 int right; // x co-ordinate of right of rectangle
};
struct GraphicsContext {
    int currentX;
    int currentY;
    int selectedPen;
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

struct planes {
    int plane0;
    int plane1;
    int plane2;
    int plane3;
};




