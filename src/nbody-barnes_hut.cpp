#include <cmath>
#include <vector>
#include <cstddef>
#include <fstream>
#include <cassert>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
using namespace std;

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

int num_thread, iters, num_body, mf;
double mass, t, angle, xmin, ymin, window_len;
bool gui;
Display* display;
Window window;
GC gc;
int screen, blackcolor, whitecolor;

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
Body *bodies, *new_bodies;

const double G = 6.67384e-11;
const int QUAD = 4;
double Gm;

void init_window(int);
void draw_points();
void draw_lines(double, double, double, double);
Vector f_with(Body& a, Body& b, double M);

class QuadTree {
private:
    enum Quadrant { NE, NW, SW, SE };
    enum NodeType { External, Internal };
    NodeType node_type;
    Vector left_upper, right_lower, mid;
    vector<QuadTree> quadrants;
    Body* content;
    Body mass_center;
    int num_body;
    double sum_mass, region_width, theta;

public:

    QuadTree() : node_type(External), num_body(0), content(nullptr) {}

    void set_region(Vector s, Vector e) {
        left_upper = s, right_lower = e; theta = angle;
        mid = { (s.x + e.x) / 2, (s.y + e.y) / 2 };
        region_width = s.y - e.y;
        quadrants.clear();
        content = nullptr;
        num_body = 0; node_type = External;
        mass_center = {0, 0}; sum_mass = 0;
    }

    Quadrant get_quadrant(Body& n) {
        double x = n.x, y = n.y;
        return (x >= mid.x) ? (y >= mid.y ? NE : SE) : (y >= mid.y ? NW : SW);
    }

    void create_quradrant() {
        if (gui) {
            draw_lines(left_upper.x, mid.y, right_lower.x, mid.y);
            draw_lines(mid.x, left_upper.y, mid.x, right_lower.y);
        }
        quadrants.resize(QUAD);
        quadrants[NE].set_region({mid.x, left_upper.y}, {right_lower.x, mid.y});
        quadrants[NW].set_region(left_upper, mid);
        quadrants[SW].set_region({left_upper.x, mid.y}, {mid.x, right_lower.y});
        quadrants[SE].set_region(mid, right_lower);
    }

    void insert(Body& node, int level) {
        if (node_type == Internal) {
            quadrants[get_quadrant(node)].insert(node, level+1);
        } else if (node_type == External && num_body == 1) {
            create_quradrant();
            node_type = Internal;
            quadrants[get_quadrant(*content)].insert(*content, level+1);
            quadrants[get_quadrant(node)].insert(node, level+1);
        } else {
            DEBUG("Insert " << node << " @lv." << level);
            content = &node;
        }
        double mx = sum_mass * mass_center.x + mass * node.x,
               my = sum_mass * mass_center.y + mass * node.y;
        sum_mass += mass;
        mass_center = { mx / sum_mass, my / sum_mass };
        num_body++;
    }

    Vector comute_force(Body& body) {
        Vector f = {0, 0};
        if (node_type == External) {
            if (content == &body) return f;
            return f_with(body, *content, mass);
        } else if (region_width /
                    pow(pow(mass_center.x - body.x, 2) +
                        pow(mass_center.y - body.y, 2), 0.5) < theta) {
            return f_with(body, mass_center, sum_mass);
        } else {
            for (int i = 0; i < QUAD; ++i) {
                if (quadrants[i].num_body < 1) continue;
                Vector t = quadrants[i].comute_force(body);
                f.x += t.x; f.y += t.y;
            }
            return f;
        }
    }
};

Vector f_with(Body& a, Body& b, double M) {
    double GMm = Gm * M, f_x, f_y;
    double dx = b.x - a.x, dy = b.y - a.y,
           radius_square = pow(dx, 2) + pow(dy, 2),
           radius_cube_sqrt = pow(radius_square, 1.5) + 10e-7;
    f_x = GMm * dx / radius_cube_sqrt;
    f_y = GMm * dy / radius_cube_sqrt;
    return { f_x, f_y };
}

void build_tree(QuadTree& tree)
{
    double _min = 0, _max = 0;
    for (int i = 0; i < num_body; ++i) {
        Body& t = bodies[i];
        _min = min(_min, min(t.x, t.y));
        _max = max(_max, max(t.x, t.y));
    }
    if (gui) {
        XClearWindow(display, window);
        XSetForeground(display, gc, 0x3C084B);
        draw_lines(_min, _max, _max, _max);
        draw_lines(_min, _min, _max, _min);
        draw_lines(_min, _min, _min, _max);
        draw_lines(_max, _min, _max, _max);
    }
    tree.set_region({_min, _max}, {_max, _min});
    for (int i = 0; i < num_body; ++i) tree.insert(bodies[i], 0);
    if (gui) draw_points();
}

void move_nth_body(int index, Vector f)
{
    Body &a = bodies[index], &new_a = new_bodies[index];
    new_a.vx = a.vx + f.x * t / mass;
    new_a.vy = a.vy + f.y * t / mass;
    new_a.x  = a.x + new_a.vx * t;
    new_a.y  = a.y + new_a.vy * t;
}

void calc(QuadTree& root)
{
    Gm = G * mass;
    for (int i = 0; i < iters; ++i) {
        build_tree(root);
        for (int j = 0; j < num_body; ++j)
            move_nth_body(j, root.comute_force(bodies[j]));
        Body* t = new_bodies; new_bodies = bodies; bodies = t;
    }
}

int main(int argc, char const **argv)
{
    num_thread = stoi(argv[1]), iters = stoi(argv[3]);
    mass = stod(argv[2]), t = stod(argv[4]), angle = stod(argv[6]);
    gui = argv[7] == string("enable");
    double len;
    if (gui) {
        xmin = stod(argv[8]), ymin = stod(argv[9]);
        len = stod(argv[10]), window_len = stoi(argv[11]);
        mf = (double) window_len / len;
        init_window(window_len);
    }
    QuadTree root = QuadTree();
    ifstream input;
    input.open(argv[5]);
    input >> num_body;
    bodies = new Body[num_body], new_bodies = new Body[num_body];
    for (int i = 0; i < num_body; ++i) {
        Body& t = bodies[i];
        input >> t.x >> t.y >> t.vx >> t.vy;
    }
    input.close();
    calc(root);
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
    XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinRound);
}

void draw_lines(double a, double b, double c, double d)
{
    XDrawLine(display, window, gc, (a-xmin)*mf, (b-ymin)*mf, (c-xmin)*mf, (d-ymin)*mf);
}

void draw_points()
{
    XSetForeground(display, gc, whitecolor);
    for (int i = 0; i < num_body; ++i) {
        Body& t = bodies[i];
        XDrawPoint(display, window, gc, (t.x - xmin) * mf, (t.y - ymin) * mf);
    }
    XFlush(display);
}
