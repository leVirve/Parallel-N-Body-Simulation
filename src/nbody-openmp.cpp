#include <omp.h>
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
struct Vector { double x, y; };
body *bodies, *new_bodies;

const double G = 6.67384e-11;
double Gmm;

void init_window(int);
void draw_points();

void diff_v(int index, Vector& v)
{
    body &a = bodies[index];
    double f_sum_x = 0, f_sum_y = 0;
    #pragma omp parallel for reduction(+:f_sum_x, f_sum_y)
    for (int i = 0; i < num_body; ++i) {
        if (index == i) continue;
        body &b = bodies[i];
        double dx = b.x - a.x, dy = b.y - a.y,
               radius_cube_sqrt = pow(pow(dx, 2) + pow(dy, 2), 1.5);
        f_sum_x +=  Gmm * dx / radius_cube_sqrt;
        f_sum_y +=  Gmm * dy / radius_cube_sqrt;
    }
    v = { f_sum_x * t / mass , f_sum_y * t / mass };
}

void calc()
{
    Gmm = G * mass * mass;
    for (int i = 0; i < iters; ++i) {
        if (gui) draw_points();
        #pragma omp for
        for (int j = 0; j < num_body; ++j) {
            Vector dv;
            diff_v(j, dv);
            body &new_b = new_bodies[j], &b = bodies[j];
            new_b.vx = b.vx + dv.x;
            new_b.vy = b.vy + dv.y;
            new_b.x  = b.x + new_b.vx * t;
            new_b.y  = b.y + new_b.vy * t;
        }
        body* t = new_bodies;
        new_bodies = bodies; bodies = t;
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
    omp_set_num_threads(num_thread);

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
