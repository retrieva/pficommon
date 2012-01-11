APPNAME = 'pficommon'
VERSION = '1.3.1'

top = '.'
out = 'build'

import Options
import sys
import os

subdirs = 'src tools'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')
  opt.load('gnu_dirs')
  
  opt.recurse(subdirs)

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('unittest_gtest')
  conf.check_tool('gnu_dirs')

  conf.env.append_unique(
    'CXXFLAGS',
    ['-O2', '-Wall', '-g', '-pipe', '-D_REENTRANT', '-fno-omit-frame-pointer', '-D_FORTIFY_SOURCE=1'])

  conf.env.HPREFIX = conf.env.PREFIX + '/include/pficommon'

  conf.recurse(subdirs)

  conf.define('PFICOMMON_VERSION', VERSION)

  conf.env['VERSION'] = VERSION
  
  conf.write_config_header('src/pfi-config.h')
  
  print("""
pficommon has been configured as follows:

[Modules]
FCGI module:             %s
Database module:         %s
  have MySQL lib:          %s
  have PostgreSQL lib:     %s
MessagePack RPC module:  %s

[Visualization]
Magick++ impl:           %s

[Build information]
Package:                 %s
build (compile on):      %s
host endian:             %s
Compiler:                %s
Compiler version:        %s
CXXFLAGS:                %s
""" % (conf.env.BUILD_FCGI and 'yes' or 'no',
       (not Options.options.disable_database) and 'yes' or 'no',
       conf.env.BUILD_MYSQL and 'yes' or 'no',
       conf.env.BUILD_PGSQL and 'yes' or 'no',
       conf.env.BUILD_MPRPC and 'yes' or 'no',
       conf.env.BUILD_MAGICKPP and 'yes' or 'no',
       APPNAME + '-' + VERSION,
       conf.env.DEST_CPU + '-' + conf.env.DEST_OS,
       sys.byteorder,
       conf.env.COMPILER_CXX,
       '.'.join(conf.env.CC_VERSION),
       ' '.join(conf.env.CXXFLAGS)))

def build(bld):
  bld.install_files('${PREFIX}/include/pficommon', [
      'src/pfi-config.h',
      ])

  bld(source = 'pficommon.pc.in',
      prefix = bld.env['PREFIX'],
      exec_prefix = '${prefix}',
      libdir = bld.env['LIBDIR'],
      includedir = '${prefix}/include',
      PACKAGE = APPNAME,
      VERSION = VERSION)

  bld.install_files(os.path.join(bld.env['LIBDIR'], 'pkgconfig'), 'pficommon.pc')
  
  bld.recurse(subdirs)
