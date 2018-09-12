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
#include "http_req.h"

AUTH_CONTEXT auth_ctx;

const char *host_name = "MacBookPro"; // 主机名字
const char *ostag = "macOS10.13.4"; // 系统标识
const char *user_agent = "CCTP/mac1/5007"; // CCTP User-Agent
const char *algo_id = "54EB0E0D-58FE-46E2-8629-0A517E2785F4"; // 编解码套件ID

#define DEF_RETRY_COUNT -1
#define DEF_RETRY_DELAY 30

void sig_handler(int sig) {
    //信号处理，处理Ctrl-C或者kill，先下线再退出（刚开始一直没写这个，测试的时候只能用其他终端把本机踢掉T.T。。。）
    printf("SIGTERM / SIGKILL captured, exiting...\n");
    exit(auth_logout(&auth_ctx));
}

int main(int argc, char *argv[]) {
    printf("** EsDialerGD by ChiL.\n");
    printf("** Build date: 2018-09-14\n");
    printf("** Project: https://github.com/claw6148/EsDialerGD\n\n");

    char *userid = getenv("ESD_USERID"); // 用户名
    char *passwd = getenv("ESD_PASSWD"); // 密码

    if (!(userid && passwd)) {
        dbgout("Insufficient environment variables, check your configuration.");
        return -1;
    }

    // 下面这个CDC三兄弟还不知道怎么动态获取，建议抓包看，在HTTP Headers里。
    char *cdc_domain = getenv("ESD_CDC_DOMAIN"); // 字符，学校拼音首字母小写
    char *cdc_area = getenv("ESD_CDC_AREA"); // 数字，区号（？）佛山是0757
    char *cdc_schoolid = getenv("ESD_CDC_SCHOOLID"); // 数字，数字

    char *retry_count_val = getenv("ESD_RETRY_COUNT");
    int retry_count = retry_count_val ? atoi(retry_count_val) : DEF_RETRY_COUNT;

    char *retry_delay_val = getenv("ESD_RETRY_DELAY");
    int retry_delay = retry_delay_val ? atoi(retry_delay_val) : DEF_RETRY_DELAY;

    // 自定义接口URL，已指定的不再自动获取
    char *ticket_url = getenv("ESD_TICKET_URL");
    char *auth_url = getenv("ESD_AUTH_URL");
    char *keep_url = getenv("ESD_KEEP_URL");
    char *term_url = getenv("ESD_TERM_URL");
    char *ipv4_addr = getenv("ESD_IPV4_ADDR");

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    int retry = 0;
    for (;;) {
        dbgout("Start dialing...");
        memset(&auth_ctx, 0, sizeof(auth_ctx));
        auth_manual_set_config(&auth_ctx,
                               ticket_url,
                               auth_url,
                               keep_url,
                               term_url,
                               ipv4_addr);
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
        dbgout("Logged in!");
        long interval = 1;
        uint32_t cnt;
        for (cnt = 1;; ++cnt) {
            //心跳包
            if (cnt % interval == 0) {
                interval = auth_keep(&auth_ctx);
                --interval; // 蛤蛤-1s，防止心跳恰好超时
                if (interval < 0) break;
                dbgout("Keep-alive in %ld second(s).", interval);
            }
            sleep(1);
        }
        if (auth_logout(&auth_ctx)) goto _fail;
        dbgout("Logged out!");
        goto _retry;
        _fail:
        dbgout(FAILED_STR);
        _retry:
        if (retry_delay <= 0) break;
        if (retry_count != -1 && (++retry) >= retry_count) break;
        if (retry_count == -1) {
            dbgout("Retry in %d second(s).", retry_delay);
        } else {
            dbgout("(%d/%d) Retry in %d second(s).", retry, retry_count, retry_delay);
        }
        sleep(retry_delay);
    }
    return 0;
}

