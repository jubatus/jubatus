import Options

VERSION = '0.2.0-alpha'
APPNAME = 'jubatus'

top = '.'

subdirs = 'src'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')

  opt.add_option('--disable-zookeeper',
                 action='store_true', default=False, # dest='nozk',
                 help='do not use ZooKeepr')
  
  opt.recurse(subdirs)

def configure(conf):
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe']

  conf.load('compiler_cxx')
  conf.load('unittest_gtest')

  conf.check_cxx(lib = 'msgpack', libpath = '/usr/local/lib')
  conf.check_cxx(lib = 'glog', libpath = '/usr/local/lib')
  
  conf.check_cfg(package = 'pficommon', args = '--cflags --libs')
  conf.check_cxx(header_name = 'pficommon/network/mprpc.h')

  conf.check_cxx(header_name = 'unistd.h')
  conf.check_cxx(header_name = 'signal.h')
  conf.check_cxx(header_name = 'sys/types.h')
  conf.check_cxx(header_name = 'sys/wait.h')
  conf.check_cxx(header_name = 'sys/stat.h')
  conf.check_cxx(header_name = 'stdint.h')
  conf.check_cxx(header_name = 'float.h') # duplicate..
  conf.check_cxx(header_name = 'assert.h')
  conf.check_cxx(header_name = 'cxxabi.h')
  conf.check_cxx(header_name = 'errno.h')
  conf.check_cxx(header_name = 'string.h')
  conf.check_cxx(header_name = 'limits.h')
  conf.check_cxx(header_name = 'sys/socket.h net/if.h')
  conf.check_cxx(header_name = 'sys/ioctl.h')
  conf.check_cxx(header_name = 'fcntl.h')
  conf.check_cxx(header_name = 'utility')
  conf.check_cxx(header_name = 'netinet/in.h')
  conf.check_cxx(header_name = 'arpa/inet.h')
  conf.check_cxx(header_name = 'dlfcn.h')
  conf.check_cxx(header_name = 'time.h')
  conf.check_cxx(header_name = 'cstdio')
  conf.check_cxx(header_name = 'cstdlib')
  conf.check_cxx(header_name = 'cmath')
  conf.check_cxx(header_name = 'cfloat')
  conf.check_cxx(header_name = 'csignal')
  conf.check_cxx(header_name = 'cerrno')
  conf.check_cxx(header_name = 'algorithm')
  conf.check_cxx(header_name = 'vector')
  conf.check_cxx(header_name = 'map')
  conf.check_cxx(header_name = 'deque')
  conf.check_cxx(header_name = 'queue')
  conf.check_cxx(header_name = 'set')
  conf.check_cxx(header_name = 'string')
  conf.check_cxx(header_name = 'iostream')
  conf.check_cxx(header_name = 'fstream')
  conf.check_cxx(header_name = 'exception')
  conf.check_cxx(header_name = 'stdexcept')
  conf.check_cxx(header_name = 'sstream')
  conf.check_cxx(header_name = 'functional')

  if not Options.options.disable_zookeeper:
    conf.check_cxx(header_name = 'c-client-src/zookeeper.h',
                   define_name = 'HAVE_ZOOKEEPER_H',
                   errmsg = 'not found ("--disable-zookeeper" option is available)')
    conf.check_cxx(lib = 'zookeeper_mt', errmsg = 'ZK not found', libpath = '/usr/local/lib')

  conf.check_cxx(lib = 'dl')

  # don't know why this does not work when put after conf.recurse
  conf.define('JUBATUS_VERSION', VERSION)
  conf.define('JUBATUS_APPNAME', APPNAME)

  conf.recurse(subdirs)

def build(bld):
  bld.recurse(subdirs)

