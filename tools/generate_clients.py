import subprocess
import sys
import os
import tarfile
import shutil

def generate(idl, lang, indir, outdir0):
    outdir = outdir0 + '/' + lang
    try: os.mkdir(outdir)
    except: pass
    print "generating %s.%s" % (idl, lang)
    idlfile = idl+".idl"
    options = []
    if lang == "cpp":
        options.append("-p")
        options.append("-n")
        options.append("jubatus")
    if lang == "ruby":
        options.append("-m")
        options.append("jubatus")
    if lang == "java":
        options.append("-p")
        options.append("jubatus")

    cmd = ["mpidl", lang, indir+idlfile, '-o', outdir] + options
    print cmd
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
    write_version(version_string, outdir+'/VERSION')

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
    langs = ["cpp", "ruby", "java", "php", "perl", "python"] # haskell is unavailable now
    servers = ["classifier", "regression", "recommender", "stat"]
    comb = [ (s, l) for s in servers for l in langs ]
    map(lambda (x,y): generate(x,y, indir, outdir), comb)

    pack(outdir)
