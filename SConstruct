import os
import sys

# unix, linux, mac  : 'clear'
# microsoft windows : 'CLS'
os.system('clear' if os.name is 'posix' else 'CLS')

BASEPATH = Dir('.').abspath



CPPPATH = [
    'libgtest/googletest-release-1.8.0/googletest/include',
    'libgtest/googletest-release-1.8.0/googlemock/include',
    'libv8/v8/include',
    'src',
  ]

source = [
    'src/context.cpp',
  ]

test_source = [
    #'libgtest/googletest-release-1.8.0/googlemock/src/gmock_main.cc',
    'src/context_test.cpp',
  ]

LIBS = [
    'libboost_chrono-mt-s',
    'libboost_system-mt-s',
    'libboost_thread-mt-s',
    'v8_base',
    'v8_init',
    'v8_initializers',
    'v8_libbase',
    'v8_libplatform',
    'v8_libsampler',
    'v8_nosnapshot',
    'v8_snapshot',
    'icuuc',
    'icui18n',
  ]

TEST_LIBS = [
    'gtest', 
  ]

LIBPATH = [
    'libboost/boost_1_66_0/lib',
    'libv8/v8/out.gn/x64.release/obj',
    'libv8/v8/out.gn/x64.release/obj/third_party/icu',
  ]

TEST_LIBPATH = [
    'libgtest',
  ]



env = Environment(CPPPATH=CPPPATH, CXXFLAGS=['-std=c++14', '-g'])
if os.name == 'posix':
  env.Append(LINKFLAGS=['-pthread'])



test = env.Program(target='unittest', source=(source + test_source), LIBS=(LIBS + TEST_LIBS), LIBPATH=(LIBPATH + TEST_LIBPATH))
Command(
  'example_unittest',
  test,
  lambda env, target, source: os.system(source[0].abspath))

env.Program(target='example', source=(source + ['src/main.cpp']), LIBS=LIBS, LIBPATH=LIBPATH)
