import Options

VERSION = '0.3.1'
APPNAME = 'jubatus'

top = '.'
out = 'build'
subdirs = 'src'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')
  opt.load('gnu_dirs')

  opt.add_option('--enable-zookeeper',
                 action='store_true', default=False, # dest='nozk',
                 help='use ZooKeeper')

  opt.add_option('--enable-gcov',
                 action='store_true', default=False,
                 dest='gcov', help='only for debug')

  opt.add_option('--enable-zktest',
                 action='store_true', default=False, 
                 dest='zktest', help='zk should run in localhost:2181')

  opt.recurse(subdirs)

def configure(conf):
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe']

  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.load('gnu_dirs')

  conf.check_cxx(lib = 'msgpack')
  conf.check_cxx(lib = 'dl')

  conf.check_cfg(package = 'libglog', args = '--cflags --libs')
  if not conf.check_cfg(package = 'libevent', args = '--cflags --libs', mandatory = False):
    conf.check_cxx(lib = 'event', uselib_store = 'LIBEVENT')
  
  conf.check_cfg(package = 'pficommon', args = '--cflags --libs')
  conf.check_cxx(header_name = 'pficommon/network/mprpc.h')

  conf.check_cxx(header_name = 'unistd.h')
  conf.check_cxx(header_name = 'sys/types.h')
  conf.check_cxx(header_name = 'sys/wait.h')
  conf.check_cxx(header_name = 'sys/stat.h')
  conf.check_cxx(header_name = 'cxxabi.h')
  conf.check_cxx(header_name = 'sys/socket.h net/if.h')
  conf.check_cxx(header_name = 'sys/ioctl.h')
  conf.check_cxx(header_name = 'fcntl.h')
  conf.check_cxx(header_name = 'netinet/in.h')
  conf.check_cxx(header_name = 'arpa/inet.h')
  conf.check_cxx(header_name = 'dlfcn.h')

  if Options.options.enable_zookeeper:
    if (conf.check_cxx(header_name = 'c-client-src/zookeeper.h',
                           define_name = 'HAVE_ZOOKEEPER_H',
                           mandatory = False)):
      conf.define('ZOOKEEPER_HEADER', 'c-client-src/zookeeper.h')
    else:
      conf.check_cxx(header_name = 'zookeeper/zookeeper.h',
                     define_name = 'HAVE_ZOOKEEPER_H',
                     errmsg = 'not found ("--disable-zookeeper" option is available)',
                     mandatory = True)
      conf.define('ZOOKEEPER_HEADER', 'zookeeper/zookeeper.h')

    conf.check_cxx(lib = 'zookeeper_mt', errmsg = 'ZK not found')

    if Options.options.zktest:
      conf.env.INTEGRATION_TEST = True

  if Options.options.gcov:
    conf.env.append_value('CXXFLAGS', '-fprofile-arcs')
    conf.env.append_value('CXXFLAGS', '-ftest-coverage')
    conf.env.append_value('LINKFLAGS', '-lgcov')

  # don't know why this does not work when put after conf.recurse
  conf.define('JUBATUS_VERSION', VERSION)
  conf.define('JUBATUS_APPNAME', APPNAME)
  conf.define('BUILD_DIR',  conf.bldnode.abspath())
  conf.write_config_header('src/config.hpp', remove=False)

  conf.recurse(subdirs)

def build(bld):

  bld(source = 'jubatus.pc.in',
      prefix = bld.env['PREFIX'],
      exec_prefix = '${prefix}',
      libdir = bld.env['LIBDIR'],
      includedir = '${prefix}/include',
      PACKAGE = APPNAME,
      VERSION = VERSION)

  bld.install_files('${PREFIX}/lib/pkgconfig', 'jubatus.pc')
  bld.recurse(subdirs)

