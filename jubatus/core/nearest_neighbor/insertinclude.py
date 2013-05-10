#!/usr/bin/env python

import os
import sys

def usage():
    print "insertinclude.py PROJECT FILE1 [FILE2] .."

if len(sys.argv) < 3:
    usage()
    sys.exit(1)

project = sys.argv[1]
files = sys.argv[2:]

def insert_include(project, file):
    path = os.path.abspath(file)
    dirs = path.split('/')

    if 'src' in dirs:
        begin = dirs.index('src') + 1
    else:
        begin = 0
    var = (project + '_' + '_'.join(dirs[begin:])).upper().replace('.', '_') + '_'

    lines = []
    with open(file) as f:
        for line in f:
            lines.append(line)

    inserted = False
    prev_ifdef = False
    prev_blank_line = False
    with open(file, 'w') as f:
        for l in lines:
            blank = (l.strip() == '')
            if not blank and prev_ifdef:
                f.write('\n')

            if "#pragma once" in l:
                f.write('#ifndef %s\n' % var)
                f.write('#define %s\n' % var)
                prev_ifdef = True
                inserted = True
            else:
                prev_ifdef = False
                f.write(l)
            prev_blank_line = blank

        if inserted:
            if not prev_blank_line:
                f.write('\n')
            f.write('#endif  // %s\n' % var)



for file in files:
    insert_include(project, file)
