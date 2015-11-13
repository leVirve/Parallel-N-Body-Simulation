#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <sys/time.h>

#ifdef _DEBUG
#define DEBUG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG(str) do { } while ( false )
#endif
#ifndef INFO
#define INFO(str) do { std::cout << str << std::endl; } while( false )
#else
#define INFO(str) do { } while ( false )
#endif

#define MICRO_SEC 1000000
#define time_diff(x) x.tv_sec - start.tv_sec + (double)(x.tv_usec - start.tv_usec) / MICRO_SEC
#define dump_status()  struct timeval timestamp; \
                    gettimeofday(&timestamp, NULL); \
                    printf("@(time: %.12f)\n", time_diff(timestamp)); \
                    fflush(stdout);

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
void draw_points(int);
void draw_lines(double, double, double, double);

void clear_display();
void draw_colored_points();

#endif
