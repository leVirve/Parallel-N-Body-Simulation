#include "utils.h"
using namespace std;

Display* display;
Window window;
GC gc;
int screen, blackcolor, whitecolor, window_len, mf;
double xmin, ymin;

void init_env(int count, const char**argv)
{
    if (count < 7) {
        puts("Usage: ./$out threads m T t input angle enable/disable xmin ymin len L");
    }
    double len;
    num_thread = stoi(argv[1]), iters = stoi(argv[3]);
    mass = stod(argv[2]), t = stod(argv[4]);
    gui = argv[7] == string("enable");
    if (gui) {
        xmin = stod(argv[8]), ymin = stod(argv[9]);
        len = stod(argv[10]), window_len = stoi(argv[11]);
        mf = (double) window_len / len;
        init_window(window_len);
    }
    input_bodies(argv[5]);
}

void input_bodies(string filename)
{
    ifstream input;
    input.open(filename);
    input >> num_body;
    bodies = new Body[num_body];
    new_bodies = new Body[num_body];

    for (int i = 0; i < num_body; ++i) {
        Body& t = bodies[i];
        input >> t.x >> t.y >> t.vx >> t.vy;
    }
    input.close();
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
}

void draw_points(int mode)
{
    if (mode == 0) XClearWindow(display, window);
    XSetForeground(display, gc, whitecolor);
    for (int i = 0; i < num_body; ++i) {
        Body& t = bodies[i];
        XDrawPoint(display, window, gc, (t.x - xmin) * mf, (t.y - ymin) * mf);
    }
    XFlush(display);
}

void draw_lines(double a, double b, double c, double d)
{
    XDrawLine(display, window, gc, (a-xmin)*mf, (b-ymin)*mf, (c-xmin)*mf, (d-ymin)*mf);
}

void clear_display()
{
    XClearWindow(display, window);
    XSetForeground(display, gc, 0x3C084B);
}
