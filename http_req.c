//
// Created by ChileungL on 23/05/2018.
//

/* HTTP请求 */

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include "http_req.h"

typedef struct {
    size_t buf_len;
    size_t pos;
    u_char *buf;
} CURL_UD;

size_t curl_recv_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    CURL_UD *ud = userdata;
    size_t len = size * nmemb;
    if (ud->pos + len < ud->buf_len) {
        memcpy(ud->buf + ud->pos, ptr, len);
        ud->pos += len;
    }
    return len;
}

int http_req_send(const char *url,
                  char headers[][HEADER_LEN],
                  const char *post_data,
                  const char *buf,
                  size_t buf_len,
                  int incl_hdr) {
    int ret = -1;
    CURL *curl = curl_easy_init();
    if (curl) {
        CURL_UD ud = {buf_len, 0, buf};
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ud);
        curl_easy_setopt(curl, CURLOPT_HEADER, incl_hdr);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_recv_cb);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
        struct curl_slist *curl_headers = NULL;
        if (headers)
            for (int i = 0; headers[i][0]; ++i) {
                curl_headers = curl_slist_append(curl_headers, headers[i]);
            }
        if (curl_headers) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
        if (post_data) curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        ret = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return ret;
}
