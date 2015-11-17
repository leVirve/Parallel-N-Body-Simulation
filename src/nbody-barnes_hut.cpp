#include <vector>
#include <cstddef>
#include <cassert>
#include <pthread.h>
#include "utils.h"
using namespace std;

bool gui, finsish = false;
double mass, t, angle;
Body *bodies, *new_bodies;
int num_thread, iters, num_body;

const int QUAD = 4;
double Gm;

int queuing_jobs = 0, num_done = 0;
pthread_mutex_t queuing;
pthread_cond_t processing, iter_fin;

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

    void insert(Body& node) {
        if (node_type == Internal) {
            quadrants[get_quadrant(node)].insert(node);
        } else if (node_type == External && num_body == 1) {
            create_quradrant();
            node_type = Internal;
            quadrants[get_quadrant(*content)].insert(*content);
            quadrants[get_quadrant(node)].insert(node);
        } else content = &node;
        double mx = sum_mass * mass_center.x + mass * node.x,
               my = sum_mass * mass_center.y + mass * node.y;
        sum_mass += mass;
        mass_center = { mx / sum_mass, my / sum_mass };
        num_body++;
    }

    Vector compute_force(Body& body) {
        if (node_type == External) {
            if (content == &body) return {0, 0};
            return f_with(body, *content, mass);
        } else if (theta && region_width /
                    pow(pow(mass_center.x - body.x, 2) +
                        pow(mass_center.y - body.y, 2), 0.5) < theta) {
            return f_with(body, mass_center, sum_mass);
        } else {
            Vector f = {0, 0};
            for (int i = 0; i < QUAD; ++i) {
                if (quadrants[i].num_body < 1) continue;
                Vector t = quadrants[i].compute_force(body);
                f.x += t.x; f.y += t.y;
            }
            return f;
        }
    }
};

QuadTree root = QuadTree();

inline Vector f_with(Body& a, Body& b, double M) {
    double GMm = Gm * M, f_x, f_y;
    double dx = b.x - a.x, dy = b.y - a.y,
           radius_cube_sqrt = pow(pow(dx, 2) + pow(dy, 2), 1.5) + 10e-7;
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
        clear_display();
        draw_lines(_min, _max, _max, _max);
        draw_lines(_min, _min, _max, _min);
        draw_lines(_min, _min, _min, _max);
        draw_lines(_max, _min, _max, _max);
    }
    tree.set_region({_min, _max}, {_max, _min});
    for (int i = 0; i < num_body; ++i) tree.insert(bodies[i]);
    if (gui) draw_points(1);
}

inline void move_nth_body(int i)
{
    Vector f = root.compute_force(bodies[i]);
    Body &a = bodies[i], &new_a = new_bodies[i];
    new_a.vx = a.vx + f.x * t / mass;
    new_a.vy = a.vy + f.y * t / mass;
    new_a.x  = a.x + new_a.vx * t;
    new_a.y  = a.y + new_a.vy * t;
}

void* worker(void* param)
{
    while (true) {
        pthread_mutex_lock(&queuing);
        while (!finsish && queuing_jobs <= 0)
            pthread_cond_wait(&processing, &queuing);
        int i = --queuing_jobs;
        pthread_mutex_unlock(&queuing);
        if (finsish) break;
        move_nth_body(i);
        pthread_mutex_lock(&queuing);
        num_done++;
        if (num_done >= num_body) pthread_cond_signal(&iter_fin);
        pthread_mutex_unlock(&queuing);
    }
}

int main(int argc, char const **argv)
{
    init_env(argc, argv);

    pthread_t workers[num_thread];
    pthread_mutex_init(&queuing, NULL);
    pthread_cond_init(&iter_fin, NULL);
    pthread_cond_init(&processing, NULL);

    for (int i = 0; i < num_thread; ++i)
        pthread_create(&workers[i], NULL, worker, NULL);

    Gm = G * mass;
    for (int i = 0; i < iters; ++i) {
        build_tree(root);
        pthread_mutex_lock(&queuing);
        queuing_jobs = num_body, num_done = 0;
        pthread_cond_broadcast(&processing);
        pthread_cond_wait(&iter_fin, &queuing);
        pthread_mutex_unlock(&queuing);
        Body* t = new_bodies; new_bodies = bodies; bodies = t;
    }
    finsish = true;
    pthread_mutex_lock(&queuing);
    pthread_cond_broadcast(&processing);
    pthread_mutex_unlock(&queuing);
    for (int j = 0; j < num_thread; ++j) pthread_join(workers[j], NULL);
    return 0;
}
