//
// Created by ChileungL on 23/05/2018.
//

/* 程序入口
 *
 * 1. 从环境变量取得一些必备参数
 * 2. 登陆、保活、登出函数的调用
 *
 *
 * 很蟾愧，这份文件只做了一些微小的工作。
 */

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "str_extract.h"
#include "dbgout.h"
#include "auth.h"

AUTH_CONTEXT auth_ctx;

const char *host_name = "MacBookPro"; // 主机名字
const char *ostag = "macOS10.12.6"; // 系统标识
const char *user_agent = "CCTP/mac1/5007"; // CCTP User-Agent
const char *algo_id = "54EB0E0D-58FE-46E2-8629-0A517E2785F4"; // 编解码套件ID

int dbgout_enable = 0;

void sig_handler(int sig) {
    //信号处理，处理Ctrl-C或者kill，先下线再退出（刚开始一直没写这个，测试的时候只能用其他终端把本机踢掉T.T。。。）
    printf("SIGTERM / SIGKILL captured, exiting...\n");
    exit(auth_logout(&auth_ctx));
}

int main(int argc, char *argv[]) {
    printf("** EsDialerCD by ChiL.\n");
    printf("** Build date: 2018-05-24\n");
    printf("** Project: https://github.com/claw6148/EsDialerGD\n\n");

    char *userid = getenv("ESD_USERID"); // 用户名
    char *passwd = getenv("ESD_PASSWD"); // 密码

    // 下面这个CDC三兄弟还不知道怎么动态获取，建议抓包看，在HTTP Headers里（好像留空也可以，export XXXX=''）
    char *cdc_domain = getenv("ESD_CDC_DOMAIN"); // 字符，学校拼音首字母小写
    char *cdc_area = getenv("ESD_CDC_AREA"); // 数字，区号（？）佛山是0757
    char *cdc_schoolid = getenv("ESD_CDC_SCHOOLID"); // 数字，数字

    if (!(userid && passwd && cdc_domain && cdc_area && cdc_schoolid)) {
        printf("Insufficient environment variables, check your configuration.\n");
        return -1;
    }

    char *retry_count_val = getenv("ESD_RETRY_COUNT");
    char *retry_delay_val = getenv("ESD_RETRY_DELAY");
    char *check_interval_val = getenv("ESD_CHECK_INTERVAL");
    dbgout_enable = (int) getenv("ESD_DEBUG");

    int retry_count = retry_count_val ? atoi(retry_count_val) : -1;
    int retry_delay = retry_delay_val ? atoi(retry_delay_val) : 30;
    long check_interval = check_interval_val ? atoi(check_interval_val) : 30;


    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    int retry=0;
    for (;;) {
        printf("Start dialing...\n");
        memset(&auth_ctx, 0, sizeof(auth_ctx));
        if (auth_init(&auth_ctx,
                      cdc_domain,
                      cdc_area,
                      cdc_schoolid,
                      ostag,
                      host_name,
                      user_agent,
                      algo_id))
            goto _fail;
        if (auth_login(&auth_ctx, userid, passwd)) goto _fail;
        printf("Logged in!\n");
        long interval = auth_keep(&auth_ctx);
        if (interval <= 0) goto _fail;
        uint32_t cnt;
        for (cnt = 0;; ++cnt) {

            //心跳包
            if (cnt % interval == 0) {
                interval = auth_keep(&auth_ctx);
                --interval; // 蛤蛤-1s，防止心跳恰好超时
                if (interval < 0) break;
                printf("Keep-alive in %ld second(s).\n", interval);
            }
            if (check_interval <= 0) check_interval = interval;

            // 连通性检查
            if (cnt % check_interval == 0) {
                AUTH_CONTEXT auth_ctx_check;
                if (auth_init(&auth_ctx_check, NULL, NULL, NULL, NULL, NULL, NULL, NULL) == 0) {
                    printf("Connectivity lost.\n");
                    break;
                }
                printf("Check connectivity in %ld second(s).\n", check_interval);
            }
            sleep(1);
        }
        if (auth_logout(&auth_ctx)) goto _fail;
        goto _retry;
        _fail:
        dbgout(FAILED_STR);
        _retry:
        if (retry_delay <= 0) break;
        if (retry_count != -1 && (++retry) >= retry_count) break;
        if (retry_count == -1)
            printf("Retry in %d second(s).\n", retry_delay);
        else
            printf("(%d/%d)Retry in %d second(s).\n", retry, retry_count, retry_delay);
        sleep(retry_delay);
        printf("\n");
    }
    return 0;
}
