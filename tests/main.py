#! /usr/bin/python2.6
# -*- coding: utf-8 -*-
import sys
import data
import subprocess as sp

def gen_detect(data):
    """
    Return a dict:
        { filename : mask, filename2 : mask2, ...}
    """

    detect = data["detect"]
    r = {}

    for (filename, m) in detect.items():
        r[filename] = "\n".join(map(lambda x: " ".join(map(str, x)), m))
    return r

def gen_read(data):
    """
    Return a dict:
        { filename : (mask, hour), filename2: (mask2, hour2), ...}
    """

    read = data["read"]
    detect = gen_detect(data)
    r = {}

    for (filename, mask) in detect.items():
        r[filename] = (mask, read[filename])
    return r

PATH_BIN = "./BIN"

def call_bin(mode, filename, mask=None):
    cmd = [PATH_BIN, mode, filename]
    stdin = sp.PIPE if mask is not None else None

    print "Exec: " + " ".join(cmd)

    #p = sp.Popen(cmd, stdout=sp.PIPE, stdin=stdin)
    #out = p.communicate(input=mask)[0]

    #return (p.returncode, out)
    return (0, "12:05")

def test_detect(data):
    results = {"test": 0, "ok": 0}

    for (filename, mask) in gen_detect(data).items():
        (ret, out) = call_bin("detect", filename)

        results["test"] += 1

        print "OUT:"
        print out
        print "SHOULD:"
        print mask

        if out == mask:
            results["ok"] += 1
    return results

def test_read(data):
    results = {"test": 0, "ok": 0}
    for (filename, (mask, hour)) in gen_read(data).items():
        (ret, out) = call_bin("read", filename, mask)

        results["test"] += 1

        print "OUT:"
        print out
        print "SHOULD:"
        print hour

        if out == hour:
            results["ok"] += 1
    return results

def test_full(data):
    results = {"test": 0, "ok": 0}
    for (filename, (mask, hour)) in gen_read(data).items():
        (ret, out) = call_bin("full", filename, mask)

        results["test"] += 1

        print "OUT:"
        print out
        print "SHOULD:"
        print hour

        if out == hour:
            results["ok"] += 1
    return results

def main(args):
    print "Detect:"
    r = test_detect(data.DATA)

    print r["ok"], "/", r["test"]
    print

    print "Read:"
    r = test_read(data.DATA)
    print r["ok"], "/", r["test"]
    print

    print "Full:"
    r = test_full(data.DATA)
    print r["ok"], "/", r["test"]
    print

if __name__ == "__main__":
    main(sys.argv)
