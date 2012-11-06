#! /usr/bin/python2.6
# -*- coding: utf-8 -*-
import sys
import math
import data
import subprocess as sp

PATH_BIN = "./clockreader"

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


def call_bin(mode, filename, mask=None):
    filename = "images/" + filename + ".jpg"
    cmd = [PATH_BIN, mode, filename]
    stdin = sp.PIPE if mask is not None else None

    print "Exec: " + " ".join(cmd)

    if mode == "detect" or mode == "read":
        p = sp.Popen(cmd, stdout=sp.PIPE, stdin=stdin)
        out = p.communicate(input=mask)[0][:-1]
        return (p.returncode, out)
    return (0, "12:05")

def test_detect(data):
    """
    Run the clock detection tests, compare the resulting masks (coordinates).
    """
    results = {"test": 0, "ok": 0}

    for (filename, mask) in gen_detect(data).items():
        (ret, out) = call_bin("detect", filename)

        results["test"] += 1

        if out == mask:
            results["ok"] += 1
        # String are different, compare the coordinates
        else:

            # string output -> numeric masks (sorted by coordinates)
            mask_sort = lambda ls: sorted(ls,
                                          cmp=lambda a, b: (a[0] < b[0] or
                                                            a[1] < b[1]))
            dist = lambda a, b : math.sqrt((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2)

            masks_should = map(lambda x: map(int, x.split(" ")), mask.split("\n"))
            masks_out = map(lambda x: map(int, x.split(" ")), out.split("\n"))

            masks_should = mask_sort(masks_should)
            masks_out = mask_sort(masks_out)

            # TODO: Compute common area between the 2 masks
            # compute distances
            dist_sum = float("inf")
            if len(masks_should) == len(masks_out):
                dist_sum = 0
                for (i, ms) in enumerate(masks_should):
                    mo = masks_out[i]
                    dist_sum += dist(ms, mo)

            # Display the error, or add a good result
            if dist_sum > 10:
                print "Got:"
                print masks_out
                print "Instead of:"
                print masks_should
            else:
                results["ok"] += 1

    return results

def test_read(data):
    results = {"test": 0, "ok": 0}
    for (filename, (mask, hours_should)) in gen_read(data).items():
        (ret, out) = call_bin("read", filename, mask)

        sort_hour = lambda ls: sorted(ls, cmp=lambda a, b: a[0] < b[0] or a[1] < b[1])

        hours_out = map(lambda x: map(int, x.split(":")), out.split("\n"))
        hours_out = sort_hour(hours_out)
        hours_should = sort_hour(hours_should)

        results["test"] += 1

        # TODO: take into account % 12 and % 60 (12:59 ~= 01:02)
        if len(hours_out) == len(hours_should):
            d = 0
            dist = lambda lsa, lsb: sum(map(lambda a, b: (a - b) ** 2, lsa, lsb))

            for (i, h) in enumerate(hours_should):
                d += dist(h, hours_out[i])

            if d < 3:
                results["ok"] += 1
            else:
                print "distance:", d
                print "Got:"
                print hours_out
                print "Instead of:"
                print hours_should
        else:
            print "distance:", d
            print "Got:"
            print hours_out
            print "Instead of:"
            print hours_should
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

    print "Finally:", r["ok"], "/", r["test"]
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
