FAS_CFLAGS = [
	'-Wall',
    '-static',
    '-std=c++11',
	'-Wno-old-style-cast'
    ]

StaticLibrary('./lib/fasutils',
        Glob("utils/*.cpp"),
        CPPPATH=['.', 'utils'],
        CCFLAGS = FAS_CFLAGS
        )
StaticLibrary('./lib/fas',
        Glob("base/*.cpp"),
        LIBPATH = ['lib', '/home/fas/boost/lib'],
        CPPPATH = ['.', 'utils', 'base', '/home/fas/'],
        LIBS = ['fasutils', 'glog', 'pthread', 'jsoncpp', 'boost_context'],
        CCFLAGS = FAS_CFLAGS
        )

Program('demo/bin/Client', 'demo/Client.cpp',
        LIBPATH = ['lib', '/home/fas/boost/lib'],
        CPPPATH = ['.', 'utils', 'base', '/home/fas/'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp', 'boost_context'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
Program('demo/bin/Server', 'demo/server.cpp',
        LIBPATH = ['lib', '/home/fas/boost/lib'],
        CPPPATH = ['.', 'utils', 'base', '/home/fas/'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp', 'boost_context'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
Program('demo/bin/LogDemo', 'demo/Logdemo.cpp',
        LIBPATH = ['lib', '/home/fas/boost/lib'],
        CPPPATH = ['.', 'utils', 'base', '/home/fas/'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp', 'boost_context'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
Program('demo/bin/CoroutineDemo', 'demo/Coroutine.cpp',
        LIBPATH = ['lib', '/home/fas/boost/lib'],
        CPPPATH = ['.', 'utils', 'base', '/home/fas/'],
        LIBS = ['fasutils', 'fas', 'glog', 'pthread', 'jsoncpp', 'boost_context'],
        CCFLAGS = ['-Wall', '-static', '-std=c++11']
       )
