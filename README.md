![Moxie logo](https://github.com/fasShare/moxie/blob/master/logo/moxie.png)
***
 moxie是Linux下基于reactor模型的框架，它主要使用C++来实现，原生支持多线程，支持poll和epoll等IO复用模型。效率，moxie与用户的交互主要采用回调函数来实现，能够很好的提高系统的数据接收与处理的异步性，提高系统并发性。接口，moxie具有不同模块对外提供的接口比较简单统一，不用在接口使用、模块间类与类之间的交互上花费太多时间，能够极大的降低用户的开发成本。模块，moxie的前身是FAS，FAS是为了学习muduo深入理解muduo而设计开发的，但考虑到muduo在某些程度上对模块分割以及代码修改还是需要在代码层面付出很大努力的，因此moxie在对大幅度FAS进行重构时，从一开始设计就保持各个模块较强的独立性，很多模块都可以拉出来单独使用，与此同时moxie也是一个对系统api封装完好的网络库。<br>
***
# moxie的主要特性<br>
* 包含常用的稳定模块<br>
  1. 多线程模块
  2. IO复用
  3. TCP链接管理
  4. 定时器
* 模块对立性强<br>
* 合理使用设计模式，可扩展性强<br>
* 接口简洁，代码可读性强<br>

# moxie依赖 <br>
* moxie对boost依赖<br>
moxie在设计过程中为了提高设置回调函数时安全性以及让这个过程更加灵活简单，主要采用了boost::function来完成回调函数的调用，因此也引入了moxie对boost的依赖。<br>
* 依赖库的安装<br>
  1. boost安装<br>
  Ubuntu下使用命令：<br>
```bash
  sudo apt-get install libboost-dev
```
  测试程序如下：<br>
```cpp
    #include <iostream> 
    #include <boost/lexical_cast.hpp>
    int main()
    {
      string s = "100";
      int a = boost::lexical_cast<int>(s);
      int b = 1;
      std::cout << a+b << std::endl;
      return 0;
    }
```
# moxie的使用
moxie早编译完后会在demo/bin目录下生成测试程序的bin文件，并且在moxie/lib下生成对应的lib文件，然后把lib以及对应头文件拷贝到自己的程序目录下，编译自己的程序时-lmoxie就可以使用了。
