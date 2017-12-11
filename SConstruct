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
        LIBS = ['fasutils', 'glog', 'pthread', 'jsoncpp'],
        CCFLAGS = FAS_CFLAGS
        )

Program('demo/bin/Client', 'demo/Client.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
Program('demo/bin/Server', 'demo/server.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
Program('demo/bin/LogDemo', 'demo/Logdemo.cpp',
        LIBPATH = ['lib'],
        CPPPATH = ['.', 'utils', 'base'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
