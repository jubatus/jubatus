import subprocess
import sys
import os
import tarfile
import shutil
import re

def generate(idl, lang, indir, outdir):

    print("generating {0}.{1}".format(idl, lang))
    idlfile = idl+".idl"
    options = []
    if lang == "cpp":
        options.append("-p")
        options.append("-n")
        options.append("jubatus")
    if lang == "ruby":
        options.append("-m")
        options.append("Jubatus")
    if lang == "java":
        outdir = outdir + '/' + idl
        try:     os.mkdir(outdir)
        except:  pass
        options.append("-p")
        options.append("jubatus")
    if lang == "haskell":
        try:
            os.mkdir(outdir + '/' + idl)
        except:
            pass
        outdir = outdir + '/' + idl

    cmd = ["mpidl", lang, indir + idlfile, '-o', outdir] + options
#    print(cmd)
    subprocess.call(cmd)
    print(outdir)
    return outdir

def get_version(path):
    f = open(path + "wscript")
    for line in f.readlines():
        if line[:7] == 'VERSION':
            m = re.search('[0-9\.]+', line)
            print m.group(0)
            return m.group(0)

def write_version(version_string, file):
    f = open(file, "w")
    f.write(version_string)
    f.close()

def pack(name, version, outdir):
    filename = "%s-%s.tar.gz" % (name, version)
    tar = tarfile.open(filename, "w:gz")
    tar.add(outdir)
    tar.close()

    shutil.rmtree(outdir, True)
    print("wrote %s" % filename)

if __name__=='__main__':
    version_string = get_version("../")
    indir = "../src/server/"
    if len(sys.argv) > 1:
        indir = sys.argv[1] + "/"

    langs = ["haskell", "cpp", "ruby", "java", "php", "perl", "python"]
    servers = ["classifier", "regression", "recommender", "stat"]
    comb = [(s, l) for s in servers for l in langs]
    for lang in langs:
        outdir = "jubatus"
        try: os.mkdir(outdir)
        except: pass

        for server in servers:
            generate(server, lang, indir, outdir)
        
        name = "jubatus-%s" % lang
        pack(name, version_string, outdir)
