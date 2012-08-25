{
  'target_defaults': {
  },

  'targets': [
    {
      'target_name': 'wpoll',
      'type': '<(library)',

      'libraries': ['-lws2_32.lib' ],
      'include_dirs': [ 'include', 'src' ],

      'direct_dependent_settings': {
        'include_dirs': [ 'include' ],
        'libraries': ['-lws2_32.lib' ]
      },

      'sources': [
        'common.gypi',
        'include/wpoll.h',
        'src/msafd.h',
        'src/tree.h',
        'src/wpoll.c',
        'src/msafd.c'
      ]
    },

    {
      'target_name': 'test',
      'type': 'executable',

      'dependencies': [ 'wpoll' ],
      'libraries': ['-lws2_32.lib' ],
      'include_dirs': [ 'include', 'src' ],

      'sources': [
        'src/test.c'
      ]
    }
  ]
}
