#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
using namespace std;

int num_thread, iters, num_body;
double mass, t, xmin, ymin;
bool gui;
Display* display;
Window window;
GC gc;
int screen, blackcolor, whitecolor, mf;
struct body { double x, y, vx, vy; };
body *bodies, *new_bodies;

const double G = 6.67384e-11;
double Gmm;

void init_window(int);
void draw_points();

void move_nth_body(int index)
{
    body &a = bodies[index], &new_a = new_bodies[index];
    double f_sum_x = 0, f_sum_y = 0;
    for (int i = 0; i < num_body; ++i) {
        if (index == i) continue;
        body &b = bodies[i];
        double dx = b.x - a.x, dy = b.y - a.y,
               radius_cube_sqrt = pow(pow(dx, 2) + pow(dy, 2), 1.5);
        f_sum_x +=  Gmm * dx / radius_cube_sqrt;
        f_sum_y +=  Gmm * dy / radius_cube_sqrt;
    }
    new_a.vx = a.vx + f_sum_x * t / mass;
    new_a.vy = a.vy + f_sum_y * t / mass;
    new_a.x  = a.x + new_a.vx * t;
    new_a.y  = a.y + new_a.vy * t;
}

void calc()
{
    Gmm = G * mass * mass;
    for (int i = 0; i < iters; ++i) {
        if (gui) draw_points();
        for (int j = 0; j < num_body; ++j) move_nth_body(j);
        body* t = new_bodies; new_bodies = bodies; bodies = t;
    }
}

int main(int argc, char const **argv)
{
    puts("./out #threads m T t input angle enable/disable xmin ymin len L");

    num_thread = stoi(argv[1]), iters = stoi(argv[3]);
    mass = stod(argv[2]), t = stod(argv[4]);
    gui = argv[7] == string("enable");
    int window_len; double len;
    if (gui) {
        xmin = stod(argv[8]), ymin = stod(argv[9]);
        len = stod(argv[10]), window_len = stoi(argv[11]);
        mf = (double) window_len / len;
        init_window(window_len);
    }

    ifstream input;
    input.open(argv[5]);

    input >> num_body;
    bodies = new body[num_body];
    new_bodies = new body[num_body];

    double x, y, vx, vy;
    for (int i = 0; i < num_body; ++i) {
        body& t = bodies[i];
        input >> t.x >> t.y >> t.vx >> t.vy;
    }
    input.close();

    calc();
    for (int j = 0; j < num_body; ++j)
        cout <<"Body " << j
             << ": x= " << bodies[j].x <<" y= " << bodies[j].y
             << " Vx= " << bodies[j].vx << " Vy= " << bodies[j].vy << endl;
    return 0;
}

void init_window(int window_len)
{
    int border_width = 0;
    display = XOpenDisplay(NULL);
    if (display == NULL) { fprintf(stderr, "Cannot open display\n"); exit(1); }
    screen = DefaultScreen(display);
    blackcolor = BlackPixel(display, screen);
    whitecolor = WhitePixel(display, screen);
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 0, 0, window_len, window_len, border_width,
                                 blackcolor, blackcolor);
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, 0);

    XSetForeground(display, gc, whitecolor);
}

void draw_points()
{
    for (int i = 0; i < num_body; ++i) {
        body& t = bodies[i];
        XDrawPoint(display, window, gc, (t.x - xmin) * mf, (t.y - ymin) * mf);
    }
    XFlush(display);
}
