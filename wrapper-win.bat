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