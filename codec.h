//
// Created by ChileungL on 23/05/2018.
//

#pragma once

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define u_char unsigned char

void payload_encode(char *data, char *md5_hex, const u_char *buf);
void payload_decode(u_char *data, const u_char *buf, size_t buf_len);
