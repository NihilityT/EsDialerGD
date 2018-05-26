//
// Created by ChileungL on 23/05/2018.
//

/* 字符串提取
 *
 * 根据指定的前缀与后缀提取出两者之间的内容，用于解析下发的配置文件
 */

#include <string.h>

int str_extract(char *dest, const char *src,
                const char *prefix, const char *postfix)
{
	char *start, *end;
	*dest = '\0';
	
	if (!(start = strstr(src, prefix)) ||
	    !(end = strstr(start += strlen(prefix), postfix)))
		return -1;

	while (start != end)
		*dest++ = *start++;
	*dest = '\0';

	return 0;
}
