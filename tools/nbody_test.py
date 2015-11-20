#! /usr/bin/python
import re
import sys
import subprocess

titles = ['threads', 'mass', 'iters', 'interval', 'TESTCASE', 'angle']
phases = ['compute', 'build', 'i/o']
log_format = '{:12}' * 6
TIMES = 1
bh_parse = False


class Testcase:
    def __init__(self, n, m, c, t, case, a):
        self.threads = str(n)
        self.mass = str(m)
        self.iter = str(c)
        self.interval = str(t)
        self.case = str(case)
        self.angle = str(a)

    def __str__(self):
        s = self
        return log_format.format(s.threads, s.mass, s.iter, s.interval, s.case, s.angle)

combines = list()
combines += [(i, 10, 300, 1, 'test4.txt', 1) for i in range(1, 11)]

timeit = re.compile(r'(\d+) ms')


def report(s):
    t = 0
    for _ in range(TIMES):
        log = subprocess.check_output([sys.argv[1], s.threads, s.mass, s.iter, s.interval, s.case, s.angle, 'disable'])
        time = timeit.findall(log)
        t += int(time[0])
    print('{} {:8} us'.format(s, float(t) / TIMES) if not bh_parse else ('{:15}' * 3).format(*time))


def main():
    testcases = [Testcase(*c) for c in combines]
    print(log_format.format(*titles) if not bh_parse else ('{:15}' * 3).format(*phases))
    for i, s in enumerate(testcases):
        report(s, )

if __name__ == '__main__':
    bh_parse = len(sys.argv) > 2
    main()
