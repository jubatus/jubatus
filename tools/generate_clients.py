import subprocess
import sys
import os
import tarfile
import shutil

def generate(idl, lang, indir, outdir0):
    outdir = outdir0 + '/' + lang
    try:
        os.mkdir(outdir)
    except:
        pass

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
        options.append("-p")
        options.append("jubatus")
    if lang == "haskell":
        try:
            os.mkdir(outdir + '/' + idl)
        except:
            pass
        outdir = outdir + '/' + idl

    cmd = ["mpidl", lang, indir + idlfile, '-o', outdir] + options
    print(cmd)
    subprocess.call(cmd)

def get_version(path):
    f = open(path + "wscript")
    for line in f.readlines():
        if line[:7] == 'VERSION':
            return line

def write_version(version_string, file):
    f = open(file, "w")
    f.write(version_string)
    f.close()

def pack(outdir):
    version_string = get_version("../")
    write_version(version_string, outdir + '/VERSION')

    tar = tarfile.open("jubatus-clients.tar.gz", "w:gz")
    tar.add(outdir)
    tar.close()

    shutil.rmtree(outdir, True)

if __name__=='__main__':
    indir = "../src/server/"
    if len(sys.argv) > 1:
        indir = sys.argv[1] + "/"
    outdir = "jubatus-clients"
    try: os.mkdir(outdir)
    except: pass
    langs = ["haskell", "cpp", "ruby", "java", "php", "perl", "python"]
    servers = ["classifier", "regression", "recommender", "stat"]
    comb = [(s, l) for s in servers for l in langs]
    for x, y in comb:
        generate(x,y, indir, outdir)

    pack(outdir)
