# Moxie
***
### moxie是基于linux操作系统下IO复用＋多线程的Reactor模型的服务器框架，它的主要设计思想借鉴与muduo，但在代码的组织以及类与类之间的关联关系上做了很大改进使其更容易修改并进行模块的添加，与此同时在一些多线程共享的地方做了很大努力，很多线程的资源共享更加容易。<br>
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

***
![Moxie logo] (https://github.com/fasShare/moxie/blob/master/logo/moxie.png)
***
