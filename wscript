# -*- python -*-
import Options
from waflib.Errors import TaskNotReady
import os
import sys

VERSION = '0.4.5'
APPNAME = 'jubatus'

top = '.'
out = 'build'
subdirs = ['config', 'jubatus', 'plugin']

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')
  opt.load('gnu_dirs')

  opt.add_option('--enable-debug',
                 action='store_true', default=False,
                 dest='debug', help='build for debug')

  opt.add_option('--enable-zookeeper',
                 action='store_true', default=False, # dest='nozk',
                 help='use ZooKeeper')

  opt.add_option('--enable-gcov',
                 action='store_true', default=False,
                 dest='gcov', help='only for debug')

  opt.add_option('--enable-zktest',
                 action='store_true', default=False, 
                 dest='zktest', help='zk should run in localhost:2181')

  # use (base + 10) ports for RPC module tests
  opt.add_option('--rpc-test-port-base',
                 default=60023, choices=map(str, xrange(1024, 65535 - 10)),
                 help='base port number for RPC module tests')

  opt.recurse(subdirs)

def configure(conf):
  conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe'];

  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.load('gnu_dirs')

  # Generate config.hpp
  conf.env.JUBATUS_PLUGIN_DIR = conf.env['LIBDIR'] + '/jubatus/plugin'
  conf.define('JUBATUS_VERSION', VERSION)
  conf.define('JUBATUS_APPNAME', APPNAME)
  conf.define('JUBATUS_PLUGIN_DIR', conf.env.JUBATUS_PLUGIN_DIR)
  conf.write_config_header('jubatus/config.hpp', guard="JUBATUS_CONFIG_HPP_", remove=False)

  conf.check_cxx(lib = 'msgpack')
  conf.check_cxx(lib = 'jubatus_mpio')
  conf.check_cxx(lib = 'jubatus_msgpack-rpc')
  conf.check_cxx(lib = 'dl')

  # pkg-config tests
  conf.find_program('pkg-config') # make sure that pkg-config command exists
  try:
    conf.check_cfg(package = 'libglog', args = '--cflags --libs')
    conf.check_cfg(package = 'pficommon', args = '--cflags --libs')
  except conf.errors.ConfigurationError:
    e = sys.exc_info()[1]
    conf.to_log("PKG_CONFIG_PATH: " + os.environ.get('PKG_CONFIG_PATH', ''))
    conf.fatal("Failed to find the library. Please confirm that PKG_CONFIG_PATH environment variable is correctly set.", e)

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

  if not Options.options.debug:
    conf.define('NDEBUG', 1)
    conf.define('JUBATUS_DISABLE_ASSERTIONS', 1)

  if Options.options.enable_zookeeper:
    if (conf.check_cxx(header_name = 'c-client-src/zookeeper.h',
                           define_name = 'HAVE_ZOOKEEPER_H',
                           mandatory = False)):
      conf.define('ZOOKEEPER_HEADER', 'c-client-src/zookeeper.h')
    else:
      conf.check_cxx(header_name = 'zookeeper/zookeeper.h',
                     define_name = 'HAVE_ZOOKEEPER_H',
                     errmsg = 'ZooKeeper c-binding is not found. Please install c-binding.',
                     mandatory = True)
      conf.define('ZOOKEEPER_HEADER', 'zookeeper/zookeeper.h')

    conf.check_cxx(lib = 'zookeeper_mt', errmsg = 'ZK not found')

    if Options.options.zktest:
      conf.env.INTEGRATION_TEST = True

  conf.check_cfg(package='eigen3', args = '--cflags', atleast_version = '3.1.0')

  if Options.options.gcov:
    conf.env.append_value('CXXFLAGS', '-fprofile-arcs')
    conf.env.append_value('CXXFLAGS', '-ftest-coverage')
    conf.env.append_value('LINKFLAGS', '-lgcov')

  if Options.options.rpc_test_port_base:
    conf.define('JUBATUS_RPC_TEST_PORT_BASE', int(Options.options.rpc_test_port_base))

  conf.define('BUILD_DIR',  conf.bldnode.abspath())

  conf.recurse(subdirs)

def build(bld):

  bld(source = 'jubatus.pc.in',
      prefix = bld.env['PREFIX'],
      exec_prefix = '${prefix}',
      libdir = bld.env['LIBDIR'],
      includedir = '${prefix}/include',
      PACKAGE = APPNAME,
      VERSION = VERSION)

  bld(source = 'jubatus-client.pc.in',
      prefix = bld.env['PREFIX'],
      exec_prefix = '${prefix}',
      libdir = bld.env['LIBDIR'],
      includedir = '${prefix}/include',
      PACKAGE = APPNAME,
      VERSION = VERSION)

  bld(name = 'core_headers', export_includes = './')

  bld.recurse(subdirs)

def cpplint(ctx):
  import fnmatch, tempfile
  cpplint = ctx.path.find_node('tools/codestyle/cpplint/cpplint.py')
  src_dir = ctx.path.find_node('jubatus')
  file_list = []
  excludes = ['jubatus/server/third_party/*', \
              'jubatus/server/server/*_server.hpp', \
              'jubatus/server/server/*_impl.cpp', \
              'jubatus/server/server/*_proxy.cpp', \
              'jubatus/server/server/*_client.hpp', \
              'jubatus/server/server/*_types.hpp', \
              'jubatus/client/*_client.hpp', \
              'jubatus/client/*_types.hpp', \
              'jubatus/util/*.h', \
              'jubatus/util/*.cpp', \
              'jubatus/util/*/*.h', \
              'jubatus/util/*/*.cpp', \
              'jubatus/util/*/*/*.h', \
              'jubatus/util/*/*/*.cpp']
  for file in src_dir.ant_glob('**/*.cpp **/*.cc **/*.hpp **/*.h'):
    file_list += [file.path_from(ctx.path)]
  for exclude in excludes:
    file_list = [f for f in file_list if not fnmatch.fnmatch(f, exclude)]
  tmp_file = tempfile.NamedTemporaryFile(delete=True);
  tmp_file.write("\n".join(file_list));
  tmp_file.flush()
  ctx.exec_command('cat ' + tmp_file.name + ' | xargs "' + cpplint.abspath() + '" --filter=-runtime/references,-runtime/rtti 2>&1')
  tmp_file.close()

def regenerate(ctx):
  server_node = ctx.path.find_node('jubatus/server/server')
  jenerator_node = ctx.path.find_node('tools/jenerator/src/jenerator')
  if not jenerator_node:
    raise TaskNotReady('jenerator is not built yet')
  for idl_node in server_node.ant_glob('*.idl'):
    idl = idl_node.name
    service_name = os.path.splitext(idl)[0]
    jenerator_command = [jenerator_node.abspath(), '-l', 'server', '-o', '.', '-i', '-n', 'jubatus', '-g', 'JUBATUS_SERVER_SERVER_', idl]
    try:
      idl_hash = ctx.cmd_and_log(['git', 'log', '-1', '--format=%H', '--', idl], cwd=server_node.abspath()).strip()
      idl_ver = ctx.cmd_and_log(['git', 'describe', idl_hash], cwd=server_node.abspath()).strip()
      jenerator_command += ['--idl-version', idl_ver]
    except Exception:
      pass
    ctx.cmd_and_log(jenerator_command, cwd=server_node.abspath())
    print()
    if not service_name in ['graph', 'anomaly']:
      server_node.find_node('%s_client.hpp' % service_name).delete()

def regenerate_client(ctx):
  server_node = ctx.path.find_node('jubatus/server/server')
  client_node = ctx.path.find_node('jubatus/client')
  jenerator_node = ctx.path.find_node('tools/jenerator/src/jenerator')
  if not jenerator_node:
    raise TaskNotReady('jenerator is not built yet')
  for idl_node in server_node.ant_glob('*.idl'):
    idl = idl_node.name
    service_name = os.path.splitext(idl)[0]
    jenerator_command = [jenerator_node.abspath(), '-l', 'cpp', '-o', client_node.abspath(), '-i', '-n', 'jubatus::' + service_name, '-g', 'JUBATUS_CLIENT_', idl]
    try:
      idl_hash = ctx.cmd_and_log(['git', 'log', '-1', '--format=%H', '--', idl], cwd=server_node.abspath()).strip()
      idl_ver = ctx.cmd_and_log(['git', 'describe', idl_hash], cwd=server_node.abspath()).strip()
      jenerator_command += ['--idl-version', idl_ver]
    except Exception:
      pass
    ctx.cmd_and_log(jenerator_command, cwd=server_node.abspath())
    print()
