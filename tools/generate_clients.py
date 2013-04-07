#!/usr/bin/env python

import subprocess
import sys
import os
import tarfile
import shutil
import re
import glob

def generate(idlfile, lang, outdir):
    idl = os.path.splitext(os.path.basename(idlfile))[0]
    print("generating {0}.{1}".format(idl, lang))
    options = []
    if lang == "cpp":
        options.append("-p")
        options.append("-n")
        options.append("jubatus::" + idl)
    if lang == "ruby":
        options.append("-m")
        options.append("Jubatus::" + idl.capitalize())
        outdir = outdir + "/jubatus"
    if lang == "java":
        options.append("-p")
        options.append("us.jubat." + idl)
    if lang == "haskell":
        options.append("-m")
        options.append("Jubatus")
        outdir = outdir + "/" + idl
    if lang == "perl":
        options.append("-n")
        options.append("Jubatus")
        outdir = outdir + "/" + idl
    if lang == "python":
        outdir = outdir + "/jubatus"

    cmd = ["mpidl", lang, idlfile, '-o', outdir] + options
    subprocess.check_output(cmd)

def mkdir(path):
    try: os.makedirs(path)
    except: pass

def get_version(path):
    f = open(path + "wscript")
    for line in f.readlines():
        if line[:7] == 'VERSION':
            m = re.search('[0-9\.]+', line)
            print(m.group(0))
            return m.group(0)

def pack(outdir):
    filename = os.path.basename(outdir) + ".tar.gz"
    tar = tarfile.open(filename, "w:gz")
    tar.add(outdir)
    tar.close()

    shutil.rmtree(outdir, True)
    print("wrote %s" % filename)

def get_langs():
    langs = []
    usage_string = subprocess.check_output(["mpidl", "--help"])
    for line in usage_string.split("\n"):
        m = re.match('^mpidl ([a-z]+) ', line)
        if m is not None:
            langs.append(m.group(1))
    return langs

if __name__=='__main__':
    version_string = get_version("../")
    indir = "../src/server/"
    langs = get_langs()
    idlfiles = glob.glob(indir + "*.idl")

    outdir = "jubatus_client.%s" % version_string
    mkdir(outdir)

    for lang in langs:
        outsubdir = outdir + "/%s" % lang
        mkdir(outsubdir)

        for idlfile in idlfiles:
            generate(idlfile, lang, outsubdir)

    pack(outdir)
