//
// Created by ChileungL on 23/05/2018.
//

/* 接口调用
 *
 * 概念定义：
 *   【四要素】客户ID + 主机名 + User-Agent + 本地时间
 *
 * 认证流程：
 *   1. 访问任意http网址，判断是否会跳转到登陆页面（页面A）。
 *      相关函数：get_basic_info
 *
 *   2. 使用CCTP的User-Agent访问页面A，取得含有ticket.cgi（临时标识）、auth.cgi（鉴权）等接口的URL
 *      相关函数：get_config
 *
 *   3. 用【四要素】 + 内网IP + MAC地址 + 系统标识 为内容，向ticket.cgi接口发送请求
 *      若成功，则响应为ticket值。
 *      相关函数：build_ticket_payload、get_ticket
 *
 *   4. 用【四要素】 + 用户名 + 密码 + ticket值 作为内容，向auth.cgi接口发送请求，
 *      若成功，则响应为term.cgi（下线）和keep.cgi（心跳）接口的URL
 *      相关函数：build_auth_payload、send_auth
 *
 * 保活机制：
 *   1. 用【四要素】 + ticket值 作为内容，向keep.cgi发送请求
 *      如成功，则响应为interval值（心跳包的间隔周期）
 *      相关函数：build_keep_payload、send_keep
 *
 * 客户端下线：
 *   1. 用【四要素】 + ticket值 + reason值（下线类型，1=主动下线，8=被踢，其他的还不知道= =）作为内容，向term.cgi发送请求
 *      若成功……who cares～
 *      相关函数：build_term_payload、send_term
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include "codec.h"
#include "http_req.h"
#include "str_extract.h"
#include "dbgout.h"
#include "auth.h"
#include "md5.h"


void build_ticket_payload(
        AUTH_CONTEXT *ctx,
        char *data,
        char *md5_hex
) {
    u_char buf[1024] = {0};
    snprintf(buf,
             1024,
             "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             "<request>"
             "<host-name>%s</host-name>"
             "<user-agent>%s</user-agent>"
             "<client-id>%s</client-id>"
             "<ipv4>%s</ipv4>"
             "<ipv6></ipv6>"
             "<mac>%s</mac>"
             "<ostag>%s</ostag>"
             "<local-time>%s</local-time>"
             "</request>",
             ctx->host_name,
             ctx->user_agent,
             ctx->client_id,
             ctx->ipv4_addr,
             ctx->mac_addr,
             ctx->ostag,
             ctx->local_time
    );
    payload_encode(data, md5_hex, buf);
}

void build_auth_payload(
        AUTH_CONTEXT *ctx,
        char *data,
        char *md5_hex,
        const char *userid,
        const char *passwd
) {
    u_char buf[1024] = {0};
    snprintf(buf,
             1024,
             "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             "<request>"
             "<passwd>%s</passwd>"
             "<userid>%s</userid>"
             "<ticket>%s</ticket>"
             "<client-id>%s</client-id>"
             "<host-name>%s</host-name>"
             "<user-agent>%s</user-agent>"
             "<local-time>%s</local-time>"
             "</request>",
             passwd,
             userid,
             ctx->ticket,
             ctx->client_id,
             ctx->host_name,
             ctx->user_agent,
             ctx->local_time
    );
    payload_encode(data, md5_hex, buf);
}

void build_keep_payload(
        AUTH_CONTEXT *ctx,
        char *data,
        char *md5_hex
) {
    u_char buf[1024] = {0};
    snprintf(buf,
             1024,
             "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             "<request>"
             "<user-agent>%s</user-agent>"
             "<local-time>%s</local-time>"
             "<ticket>%s</ticket>"
             "<host-name>%s</host-name>"
             "<client-id>%s</client-id>"
             "</request>",
             ctx->user_agent,
             ctx->local_time,
             ctx->ticket,
             ctx->host_name,
             ctx->client_id
    );
    payload_encode(data, md5_hex, buf);
}

void build_term_payload(
        AUTH_CONTEXT *ctx,
        char *data,
        char *md5_hex,
        const int reason
) {
    u_char buf[1024] = {0};
    snprintf(buf,
             1024,
             "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             "<request>"
             "<user-agent>%s</user-agent>"
             "<ticket>%s</ticket>"
             "<local-time>%s</local-time>"
             "<host-name>%s</host-name>"
             "<reason>%d</reason>"
             "<client-id>%s</client-id>"
             "</request>",
             ctx->user_agent,
             ctx->ticket,
             ctx->local_time,
             ctx->host_name,
             reason,
             ctx->client_id
    );
    payload_encode(data, md5_hex, buf);
}

void build_headers(
        AUTH_CONTEXT *ctx,
        char headers[8][HEADER_LEN],
        const char *md5_hex
) {
    snprintf(headers[0], HEADER_LEN, "User-Agent: %s", ctx->user_agent);
    snprintf(headers[1], HEADER_LEN, "Algo-ID: %s", ctx->algo_id);
    snprintf(headers[2], HEADER_LEN, "Client-ID: %s", ctx->client_id);
    snprintf(headers[3], HEADER_LEN, "CDC-Domain: %s", ctx->cdc_domain);
    snprintf(headers[4], HEADER_LEN, "CDC-SchoolId: %s", ctx->cdc_schoolid);
    snprintf(headers[5], HEADER_LEN, "CDC-Area: %s", ctx->cdc_area);
    snprintf(headers[6], HEADER_LEN, "CDC-Checksum: %s", md5_hex);
    headers[7][0] = '\0';
}

void update_local_time(AUTH_CONTEXT *ctx) {
    time_t t;
    time(&t);
    struct tm *lt = localtime(&t);
    snprintf(ctx->local_time, 32, "%04d-%02d-%02d %02d:%02d:%02d",
             lt->tm_year + 1900,
             lt->tm_mon,
             lt->tm_mday,
             lt->tm_hour,
             lt->tm_min,
             lt->tm_sec);
}

int get_basic_info(AUTH_CONTEXT *ctx, const char *url, char *redir_url) {
    char buf[4096] = {0};
    char headers[][HEADER_LEN] = {{0}};
    if (http_req_send(url, headers, NULL, buf)) goto _fail;
    if (str_extract(redir_url, buf, ">location.href=\"", "\"")) goto _fail;
    if (str_extract(ctx->ipv4_addr, buf, "wlanuserip=", "\"")) {
        if (str_extract(ctx->ipv4_addr, buf, "wlanuserip=", "&")) goto _fail;
    }
    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int get_config(AUTH_CONTEXT *ctx,
               const char *url) {
    char buf[4096] = {0};
    char headers[][HEADER_LEN] = {{0},
                                  {0}};
    snprintf(headers[0], HEADER_LEN, "User-Agent: %s", ctx->user_agent);
    if (http_req_send(url, headers, NULL, buf)) goto _fail;
    if (str_extract(ctx->ticket_url, buf, "<ticket-url><![CDATA[", "]]></ticket-url>")) goto _fail;
    if (str_extract(ctx->auth_url, buf, "<auth-url><![CDATA[", "]]></auth-url>")) goto _fail;

    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int get_ticket(AUTH_CONTEXT *ctx) {
    update_local_time(ctx);
    char data[4096] = {0};
    char md5_hex[64] = {0};
    build_ticket_payload(ctx, data, md5_hex);
    char headers[8][HEADER_LEN];
    build_headers(ctx, headers, md5_hex);
    if (http_req_send(ctx->ticket_url, headers, data, data)) goto _fail;
    u_char data_plain[sizeof(data) >> 1] = {0};
    payload_decode((u_char *) data_plain, (const u_char *) data, strlen(data));
    if (str_extract(ctx->ticket, (char *) data_plain, "<ticket>", "</ticket>")) goto _fail;
    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int send_auth(AUTH_CONTEXT *ctx, const char *userid, const char *passwd) {
    update_local_time(ctx);
    char data[4096] = {0};
    char md5_hex[64] = {0};
    build_auth_payload(ctx, data, md5_hex, userid, passwd);
    char headers[8][HEADER_LEN];
    build_headers(ctx, headers, md5_hex);
    if (http_req_send(ctx->auth_url, headers, data, data)) goto _fail;
    u_char data_plain[sizeof(data) >> 1] = {0};
    payload_decode((u_char *) data_plain, (const u_char *) data, strlen(data));
    if (str_extract(ctx->keep_url, (char *) data_plain, "<keep-url><![CDATA[", "]]></keep-url>")) goto _fail;
    if (str_extract(ctx->term_url, (char *) data_plain, "<term-url><![CDATA[", "]]></term-url>")) goto _fail;
    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int send_keep(AUTH_CONTEXT *ctx, long *interval) {

    update_local_time(ctx);
    char data[4096] = {0};
    char md5_hex[64] = {0};

    build_keep_payload(ctx, data, md5_hex);
    char headers[8][HEADER_LEN];
    build_headers(ctx, headers, md5_hex);

    if (http_req_send(ctx->keep_url, headers, data, data)) goto _fail;

    u_char data_plain[sizeof(data) / 2] = {0};
    payload_decode((u_char *) data_plain, (const u_char *) data, strlen(data));

    char interval_val[16] = {0};
    if (str_extract(interval_val, (char *) data_plain, "<interval>", "</interval>")) goto _fail;
    *interval = atol(interval_val);

    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int send_term(AUTH_CONTEXT *ctx, int reason) {
    update_local_time(ctx);
    char data[4096] = {0};
    char md5_hex[64] = {0};
    build_term_payload(ctx, data, md5_hex, reason);
    char headers[8][HEADER_LEN];
    build_headers(ctx, headers, md5_hex);
    if (http_req_send(ctx->term_url, headers, data, data)) goto _fail;
    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int auth_init(AUTH_CONTEXT *ctx,
              const char *cdc_domain,
              const char *cdc_area,
              const char *cdc_schoolid,
              const char *ostag,
              const char *host_name,
              const char *user_agent,
              const char *algo_id
) {
    char redir_url[256] = {0};
    char *site_list[] = {
            "http://www.qq.com",
            "http://www.baidu.com",
            "http://www.sina.com.cn",
            "\0"
    };
    int i;
    for (i = 0; site_list[i][0]; ++i) {
        if (!get_basic_info(ctx, site_list[i], redir_url)) break;
    }
    if (site_list[i][0] == 0) goto _fail;
    if (!(cdc_domain && cdc_area && cdc_schoolid && ostag && host_name && user_agent && algo_id)) return 0;

    update_local_time(ctx);

    u_char md5_bin[HASHSIZE];
    md5(ctx->local_time, strlen(ctx->local_time), (char *) md5_bin);
    strcpy(ctx->client_id, "2B2B2B2B-2B2B-2B2B-2B2B-2B2B2B2B2B2B"); //客户端ID不能随机生成，否则有几率发生ticket接口调用失败。

    //随机MAC
    snprintf(ctx->mac_addr, 32, "%02X:%02X:%02X:%02X:%02X:%02X",
             (*((uint8_t *) md5_bin)) & 0xFE,
             (*((uint8_t *) (md5_bin + 1))),
             (*((uint8_t *) (md5_bin + 2))),
             (*((uint8_t *) (md5_bin + 3))),
             (*((uint8_t *) (md5_bin + 4))),
             (*((uint8_t *) (md5_bin + 5))));

    strcpy(ctx->cdc_domain, cdc_domain);
    strcpy(ctx->cdc_area, cdc_area);
    strcpy(ctx->cdc_schoolid, cdc_schoolid);
    strcpy(ctx->cdc_schoolid, cdc_schoolid);
    strcpy(ctx->ostag, ostag);
    strcpy(ctx->host_name, host_name);
    strcpy(ctx->user_agent, user_agent);
    strcpy(ctx->algo_id, algo_id);

    if (get_config(ctx, redir_url)) goto _fail;


    return 0;

    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int auth_login(AUTH_CONTEXT *ctx, const char *userid, const char *passwd) {
    if (get_ticket(ctx)) goto _fail;
    if (send_auth(ctx, userid, passwd)) goto _fail;
    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

long auth_keep(AUTH_CONTEXT *ctx) {
    long keep_interval;
    if (send_keep(ctx, &keep_interval)) goto _fail;
    return keep_interval;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}

int auth_logout(AUTH_CONTEXT *ctx) {
    if (send_term(ctx, 1)) goto _fail;
    return 0;
    _fail:
    dbgout(FAILED_STR);
    return -1;
}
