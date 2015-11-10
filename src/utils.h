#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>

struct Vector { double x, y;
    friend std::ostream& operator << (std::ostream& out, const Vector& b) {
        out << "(" << b.x << ", " << b.y << ")";
        return out;
    }
};
struct Body { double x, y, vx, vy;
    friend std::ostream& operator << (std::ostream& out, const Body& b) {
        out << "(" << b.x << ", " << b.y << ")" << "Vx= " << b.vx << ", Vy= " << b.vy;
        return out;
    }
};
const double G = 6.67384e-11;

extern int num_thread, iters, num_body;
extern double mass, t;
extern int screen, blackcolor, whitecolor;
extern bool gui;
extern Body *bodies, *new_bodies;

void init_env(int, const char**);
void input_bodies(std::string);
void init_window(int);
void draw_points();

void clear_display();
void draw_colored_points();

#endif
