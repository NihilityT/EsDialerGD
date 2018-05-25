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