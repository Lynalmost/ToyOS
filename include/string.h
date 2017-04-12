/*************************************************************************
  > File Name: string.h
  > Author: Ukey
  > Mail: gsl110809@gmail.com
  > Created Time: 2017年04月12日 星期三 18时39分11秒
 ************************************************************************/

#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include "types.h"

static inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		*dest++ = *src++;
	}
}

static inline void memset(void *s, uint8_t c, uint32_t len)
{
	uint8_t *dest = (uint8_t *)s;

	int i;
	for(i = 0; i < len; i++)
	{
		*dest++ = c;
	}
}

static inline void bzero(void *s, uint32_t len)
{
	memset(s, 0, len);
}

static inline int strcmp(const char *str1, const char *str2)
{
	while(*str1 == *str2)
	{
		if(*str1 == '\0')
			return 0;
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

static inline char *strcpy(char *dest, const char *src)
{
	char *cp = dest;

	while(*cp++ = *src++)
			;

	return dest;
}

static inline char *strcat(char *dest, const char *src)
{
	char *tmp = dest;

	while(*dest)
		dest++;
	while((*dest++ = *src++) != '\0')
			;
	return tmp;
}

static inline int strlen(const char* src)
{
	const char *tmp = src;

	while(*tmp++)
		;

	return (tmp - src - 1);
}

#endif
