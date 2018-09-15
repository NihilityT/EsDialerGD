//
// Created by ChileungL on 23/05/2018.
//

#pragma once

#define UD_BUF_SIZE 2048
#define HEADER_LEN 64

int http_req_send(const char *url,
                  char headers[][HEADER_LEN],
                  const char *post_data,
                  const char *buf,
                  size_t buf_len,
                  int incl_hdr);