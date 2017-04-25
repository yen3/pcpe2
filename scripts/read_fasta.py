#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import os

from multiprocessing import Pool
from multiprocessing import cpu_count

getOutIDFilename = lambda fn: os.path.splitext(fn)[0] + "_id_css.txt"
getOutSeqFilename = lambda fn: os.path.splitext(fn)[0] + "_seq_css.txt"
extractID = lambda id_line: id_line.split('|')[1]


def readEntry(fn):
    id_line = ""
    seq = ""
    f = open(fn)
    while True:
        l = f.readline()
        if l != "":
            if l.startswith('>'):
                if id_line != "" and seq != "":
                    yield seq, id_line
                id_line = l
                seq = ""
            else:
                seq += l[:-1]
        else:
            break

    if id_line and seq:
        yield seq, id_line


def readFastaSet(fn):
    seq_map = dict()
    for s, i in readEntry(fn):
        seq_map.setdefault(s, []).append(extractID(i))

    return seq_map


def writeResult(fn, seq_map):
    f_id = open(getOutIDFilename(fn), 'w')
    f_seq = open(getOutSeqFilename(fn), 'w')

    f_id.write(str(len(seq_map)) + "\n")
    f_seq.write(str(len(seq_map)) + "\n")

    for seq, id_list in seq_map.iteritems():
        f_id.write(str(len(id_list)) + " " + " ".join(id_list) + "\n")
        f_seq.write(str(len(seq)) + " " + seq + "\n")

    f_id.close()
    f_seq.close()


def run(fn):
    writeResult(fn, readFastaSet(fn))
    print "Read file " + fn


def main():
    fn_list = sys.argv[1:] if len(sys.argv) > 1 else ['example/influenza.faa']

    if len(fn_list) > 1:
        pool = Pool(processes=cpu_count())
        pool.map(run, fn_list)
        pool.terminate()
        pool.close()
    else:
        map(run, fn_list)

if __name__ == "__main__":
    main()
