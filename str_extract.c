//
// Created by ChileungL on 23/05/2018.
//

/* 字符串提取
 *
 * 根据指定的前缀与后缀提取出两者之间的内容，用于解析下发的配置文件
 */

#include <string.h>
#include <stdlib.h>

int str_extract(char *dst, char *src, const char *prefix, const char *postfix) {
    char *ptr_pre = NULL;
    char *ptr_post = NULL;
    char *target = NULL;
    int result = 0;
    if (!(ptr_pre = strstr(src, prefix))) goto _fail;
    if (!(target = strdup(ptr_pre + strlen(prefix)))) goto _fail;
    if (!(ptr_post = strstr(target, postfix))) goto _fail;
    *(ptr_post) = '\0';
    strcpy(dst, target);

    _leave:
    if (target) free(target);
    return result;

    _fail:
    result = -1;
    goto _leave;
}
