#! /usr/bin/python
import sys
import math
import random

with open('test%s.txt' % sys.argv[1], 'w') as f:
    f.write(sys.argv[1] + '\n')
    size = int(sys.argv[1]) * 10
    ssize = size / 10
    for _ in range(ssize):
        r, theta = [math.sqrt(random.randint(0, size))*math.sqrt(size),
                    2*math.pi*random.random()]
        x = r * math.cos(theta)
        y = r * math.sin(theta)
        f.write('%s %s %s %s\n' % (x / size, y / size, random.random(), random.random()))
