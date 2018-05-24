//
// Created by ChileungL on 23/05/2018.
//

#pragma once

#define HEADER_LEN 64

int http_req_send(const char *url, char headers[][HEADER_LEN], const char *post_data, const char *buf);