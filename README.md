# EsDialerGD
广东天翼校园三方客户端（登陆器）

## 简介

Written in C with libcurl

鉴权协议分析：https://4fk.me/post/gdes-auth-proto.html

## 源码

``main.c`` 程序入口

``auth.c`` 鉴权交互逻辑

``codec.c`` 通信数据编解码

``md5.c`` MD5函数

``str_extract.c`` 字符串提取函数

``http_req.c`` HTTP请求

## 使用方式

程序通过**环境变量**的方式进行参数传递

**必选变量**

- ``ESD_USERID`` 用户名

- ``ESD_PASSWD`` 密码

- ``ESD_CDC_DOMAIN`` 学校拼音首字母小写

- ``ESD_CDC_AREA`` 区号

- ``ESD_CDC_SCHOOLID`` 学校ID

（备注：``ESD_CDC_DOMAIN``、``ESD_CDC_AREA``、``ESD_CDC_SCHOOLID``的获取需要抓官方客户端的包，在HTTP Headers里，不过测试发现三者留空也不影响，下文的例子为留空。）

**可选变量**

- ``ESD_RETRY_COUNT`` 重试次数；0-不重试直接退出；(-1)-无限重试；默认值：-1

- ``ESD_RETRY_DELAY`` 重试间隔（秒）；默认值：30

- ``ESD_CHECK_INTERVAL`` 连接状态检查间隔（秒）；默认值：30

- ``ESD_DEBUG`` 输出调试信息；默认值：（空）

**例子**

将以下内容写到一个脚本文件内，保存到与EsDialer二进制文件同级目录下。

UNIX（对应``wrapper-unix.sh``）

```
#!/bin/sh

export ESD_USERID=foo
export ESD_PASSWD=bar

# 留空示例（但不能不写）
export ESD_CDC_DOMAIN=''
export ESD_CDC_AREA=''
export ESD_CDC_SCHOOLID=''

export ESD_RETRY_COUNT=-1
export ESD_RETRY_DELAY=20
export ESD_CHECK_INTERVAL=10
export ESD_DEBUG=1

# 运行
./EsDialer
```

Windows（对应``wrapper-win.bat``）

```
@echo off

set ESD_USERID=foo
set ESD_PASSWD=bar

REM 留空示例（但不能不写）
set ESD_CDC_DOMAIN=''
set ESD_CDC_AREA=''
set ESD_CDC_SCHOOLID=''

set ESD_RETRY_COUNT=-1
set ESD_RETRY_DELAY=20
set ESD_CHECK_INTERVAL=10
set ESD_DEBUG=1

REM 运行
.\EsDialer
```

## 编译方法
程序依赖libcurl，请先装好。

```
git clone https://github.com/claw6148/EsDialerGD.git
cd EsDialerGD
```

1. 本机编译：直接``make``。

2. 交叉编译：修改Makefile，``CROSS=``后面添上目标平台前缀，如``CROSS=arm-linux-androideabi-``，然后``make``。

## 问题反馈

邮箱：a@4fk.me
