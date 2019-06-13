APPNAME = 'pficommon'
VERSION = '3.0.0'

top = '.'
out = 'build'

from waflib import Options
import sys
import os
import re
import waflib

subdirs = 'src tools'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittest_gtest')
  opt.load('gnu_dirs')

  opt.add_option('--static',
                 action='store_true',
                 default=False,
                 dest='static',
                 help='build static library')

  opt.recurse(subdirs)

def configure(conf):
  conf.load('compiler_cxx')
  conf.load('unittest_gtest')
  conf.load('gnu_dirs')

  env = conf.env
  env.append_unique('CXXFLAGS', ['-O2', '-Wall', '-g', '-pipe', '-D_REENTRANT', '-fno-omit-frame-pointer'])
  ver = env.CC_VERSION
  if env.COMPILER_CXX != 'g++' or int(ver[0]) < 4 or (int(ver[0]) == 4 and int(ver[1]) < 6):
    env.append_unique('CXXFLAGS', '-D_FORTIFY_SOURCE=1')

  env.HPREFIX = env.PREFIX + '/include/pficommon'

  conf.recurse(subdirs)

  conf.define('PFICOMMON_VERSION', VERSION)

  conf.env['VERSION'] = VERSION

  conf.env['FEATURES'] = 'cxx cxxstlib' if Options.options.static else 'cxx cxxshlib'
  conf.env['OBJECTS_CXXFLAGS'] = conf.env.CXXFLAGS_cxxstlib if Options.options.static else conf.env.CXXFLAGS_cxxshlib
  
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
link:                    %s
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
       Options.options.static and 'static' or 'shared',
       conf.env.COMPILER_CXX,
       '.'.join(conf.env.CC_VERSION),
       ' '.join(conf.env.CXXFLAGS)))

def build(bld):
  bld.install_files('${PREFIX}/include/pficommon', [
      'src/pfi-config.h',
      ])

  bld.recurse(subdirs)
  libs = []
  for tasks in bld.get_build_iterator():
    if tasks == []:
      break
    for task in tasks:
      if not isinstance(task.generator, waflib.TaskGen.task_gen):
        continue
      g = task.generator
      if any(f in g.features for f in ['cxxshlib', 'cxxstlib']) and getattr(g, 'install_path', None) is not None:
        libs.append(g.target)
  ls = ''
  for l in set(libs):
    ls = ls + ' -l' + l
  if bld.env.BUILD_MPRPC:
    ls = ls + ' -lmsgpack'

  bld(source = 'pficommon.pc.in',
      prefix = bld.env['PREFIX'],
      exec_prefix = '${prefix}',
      libdir = bld.env['LIBDIR'],
      libs = ls,
      includedir = '${prefix}/include',
      PACKAGE = APPNAME,
      VERSION = VERSION)
