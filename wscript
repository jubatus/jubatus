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
  conf.check_cxx(header_name = 'sys/wait.h')

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

