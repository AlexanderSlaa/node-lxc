{
  'targets': [
    {
      'target_name': 'node-lxc',
      'sources': [ 'src/node-lxc.cpp', 'src/Container.cpp' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")","/usr/include/lxc"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'libraries': ["/usr/lib/x86_64-linux-gnu/liblxc.so.1.7.0"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}