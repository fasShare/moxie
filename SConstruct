FAS_CFLAGS = [
    '-Wall',
    '-static',
    '-std=c++11'
    ]

StaticLibrary('./lib/fasutils',
        Glob("utils/*.cpp"),
        CPPPATH=['.', 'utils'],
        CCFLAGS = FAS_CFLAGS
        )
StaticLibrary('./lib/fas',
        Glob("base/*.cpp"),
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = FAS_CFLAGS
        )

Program('demo/bin/Client', 'demo/Client.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
Program('demo/bin/Server', 'demo/server.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'jsoncpp', 'pthread'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
