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

    cmd = ["mpidl", lang, idlfile, '-o', outdir] + options
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
    idlfiles = glob.glob(indir + "*.idl")
    for lang in langs:
        outdir = "jubatus"
        try: os.mkdir(outdir)
        except: pass

        for idlfile in idlfiles:
            generate(idlfile, lang, outdir)
        
        name = "jubatus-%s" % lang
        pack(name, version_string, outdir)
