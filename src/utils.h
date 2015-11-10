#ifndef __UTILS_H__
#define __UTILS_H__

#include <cmath>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>

struct body { double x, y, vx, vy; };

const double G = 6.67384e-11;

extern int num_thread, iters, num_body;
extern double mass, t;
extern int screen, blackcolor, whitecolor;
extern bool gui;
extern body *bodies, *new_bodies;

void init_env(int, const char**);
void input_bodies(std::string);
void init_window(int);
void draw_points();

#endif
