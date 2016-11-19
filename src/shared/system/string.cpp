/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverses.
 * 
 * TeeUniverses is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverses is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#include "string.h"

#include <cstring>
#include <cctype>
#include <ctime>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#if defined(CONF_FAMILY_UNIX)
	#include <cstdarg>
	#include <cstdio>
	
	#define VSNPRINTF vsnprintf
	
	//I don't know if theses includes are needed
	#if defined(CONF_PLATFORM_MACOSX)
		#include <Carbon/Carbon.h>
	#endif
	
#elif defined(CONF_FAMILY_WINDOWS)

	#define VSNPRINTF _vsnprintf
	
	//I don't know if theses includes are needed
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <fcntl.h>
	#include <direct.h>
	#include <errno.h>
	#include <wincrypt.h>
	
#else
	#error NOT IMPLEMENTED
#endif

/* FOREIGN CODE END: TeeWorlds ****************************************/

char char_lower(char c)
{
	return tolower(c);
}

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

int str_comp(const char *a, const char *b)
{
	return strcmp(a, b);
}

int str_comp_num(const char *a, const char *b, const int num)
{
	return strncmp(a, b, num);
}

void str_copy(char *dst, const char *src, int dst_size)
{
	strncpy(dst, src, dst_size);
	dst[dst_size-1] = 0; /* assure null termination */
}

void str_append(char *dst, const char *src, int dst_size)
{
	int s = strlen(dst);
	int i = 0;
	while(s < dst_size)
	{
		dst[s] = src[i];
		if(!src[i]) /* check for null termination */
			return;
		s++;
		i++;
	}

	dst[dst_size-1] = 0; /* assure null termination */
}

void str_append_num(char *dst, const char *src, int dst_size, int num)
{
	int s = strlen(dst);
	int i = 0;
	while(s < dst_size)
	{
		if(i>=num)
		{
			dst[s] = 0;
			return;
		}
		
		dst[s] = src[i];
		if(!src[i]) /* check for null termination */
			return;
		s++;
		i++;
	}

	dst[dst_size-1] = 0; /* assure null termination */
}

int str_length(const char *str)
{
	return (int)strlen(str);
}

void str_format(char *buffer, int buffer_size, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	VSNPRINTF(buffer, buffer_size, format, ap);
	va_end(ap);

	buffer[buffer_size-1] = 0; /* assure null termination */
}

/* check if the string contains '..' (parent directory) paths */
bool str_check_pathname(const char* str)
{
	// State machine. 0 means that we're at the beginning
	// of a new directory/filename, and a positive number represents the number of
	// dots ('.') we found. -1 means we encountered a different character
	// since the last path separator (or the beginning of the string).
	int parse_counter = 0;
	while(*str)
	{
		if(*str == '\\' || *str == '/')
		{
			// A path separator. Check how many dots we found since
			// the last path separator.
			//
			// Two dots => ".." contained in the path. Return an
			// error.
			if(parse_counter == 2)
				return false;
			else
				parse_counter = 0;
		}
		else if(parse_counter >= 0)
		{
			// If we have not encountered a non-dot character since
			// the last path separator, count the dots.
			if(*str == '.')
				parse_counter++;
			else
				parse_counter = -1;
		}

		++str;
	}
	// If there's a ".." at the end, fail too.
	if(parse_counter == 2)
		return false;
	return true;
}

char *str_skip_to_whitespace(char *str)
{
	while(*str && (*str != ' ' && *str != '\t' && *str != '\n'))
		str++;
	return str;
}

char *str_skip_whitespaces(char *str)
{
	while(*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r'))
		str++;
	return str;
}

void str_timestamp(char *buffer, int buffer_size)
{
	time_t time_data;
	struct tm *time_info;

	time(&time_data);
	time_info = localtime(&time_data);
	strftime(buffer, buffer_size, "%Y-%m-%d_%H-%M-%S", time_info);
	buffer[buffer_size-1] = 0;	/* assure null termination */
}

int str_utf8_check(const char *str)
{
	while(*str)
	{
		if((*str&0x80) == 0x0)
			str++;
		else if((*str&0xE0) == 0xC0 && (*(str+1)&0xC0) == 0x80)
			str += 2;
		else if((*str&0xF0) == 0xE0 && (*(str+1)&0xC0) == 0x80 && (*(str+2)&0xC0) == 0x80)
			str += 3;
		else if((*str&0xF8) == 0xF0 && (*(str+1)&0xC0) == 0x80 && (*(str+2)&0xC0) == 0x80 && (*(str+3)&0xC0) == 0x80)
			str += 4;
		else
			return 0;
	}
	return 1;
}

static int str_utf8_isstart(char c)
{
	if((c&0xC0) == 0x80) /* 10xxxxxx */
		return 0;
	return 1;
}

int str_utf8_rewind(const char *str, int cursor)
{
	while(cursor)
	{
		cursor--;
		if(str_utf8_isstart(*(str + cursor)))
			break;
	}
	return cursor;
}

int str_utf8_forward(const char *str, int cursor)
{
	const char *buf = str + cursor;
	if(!buf[0])
		return cursor;

	if((*buf&0x80) == 0x0)  /* 0xxxxxxx */
		return cursor+1;
	else if((*buf&0xE0) == 0xC0) /* 110xxxxx */
	{
		if(!buf[1]) return cursor+1;
		return cursor+2;
	}
	else  if((*buf & 0xF0) == 0xE0)	/* 1110xxxx */
	{
		if(!buf[1]) return cursor+1;
		if(!buf[2]) return cursor+2;
		return cursor+3;
	}
	else if((*buf & 0xF8) == 0xF0)	/* 11110xxx */
	{
		if(!buf[1]) return cursor+1;
		if(!buf[2]) return cursor+2;
		if(!buf[3]) return cursor+3;
		return cursor+4;
	}

	/* invalid */
	return cursor+1;
}

const char *str_find_nocase(const char *haystack, const char *needle)
{
	while(*haystack) /* native implementation */
	{
		const char *a = haystack;
		const char *b = needle;
		while(*a && *b && tolower(*a) == tolower(*b))
		{
			a++;
			b++;
		}
		if(!(*b))
			return haystack;
		haystack++;
	}

	return 0;
}

int str_toint(const char *str) { return atoi(str); }

/* FOREIGN CODE END: TeeWorlds ****************************************/

