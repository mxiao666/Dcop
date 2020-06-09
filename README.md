# Dcop

#### 介绍
Dcop 是一个用于动态管理Linux运行时软件配置框架，以往Linux软件在运行是读取配置文件后，所有的参数不可再次调整，今天他来了Dcop能够很好的满足你在启动软件后，再次根据自己的需求去配置软件各项参数。

#### 软件架构
软件架构说明
1.  采用一对多模块化管理
2.  消息订阅发布模式
####  主要特性

1.  可控模块信息
2.  管理每个子模块
3.  模块间低耦合
4.  动态配置参数
### 核心功能
软件启动后默认开启 2323端口，通过telnet 0 2323 连接进入后台
1.  错误信息快速查询
![输入图片说明](https://images.gitee.com/uploads/images/2020/0609/223600_b6729f7a_1287788.png)
2.  自带系统部分必备配置查询功能
###### dump模块信息
![输入图片说明](https://images.gitee.com/uploads/images/2020/0609/223130_af398cf4_1287788.png)
###### dump notify模块
![输入图片说明](https://images.gitee.com/uploads/images/2020/0609/224059_c665d041_1287788.png)
###### 帮助信息
![输入图片说明](https://images.gitee.com/uploads/images/2020/0609/223159_94223a9b_1287788.png)

3.  基于EasyLogger 定制化

###  文档
具体内容参考 os/src/log.cpp,该模块给出了绝大部分使用方式例子


#### 码云特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  码云官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解码云上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是码云最有价值开源项目，是码云综合评定出的优秀开源项目
5.  码云官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  码云封面人物是一档用来展示码云会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
