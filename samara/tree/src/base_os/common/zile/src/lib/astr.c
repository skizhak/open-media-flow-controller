/*	$Id: astr.c,v 1.1.1.1 2003/09/19 17:05:34 gregs Exp $	*/

/*
 * Copyright (c) 2001 Sandro Sigala.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef TEST
#undef NDEBUG
#endif
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "astr.h"

#define ALLOCATION_CHUNK_SIZE	16

static void resize_if_smaller(astr as, size_t reqsize)
{
	assert(as != NULL);
	if (reqsize > as->maxsize) {
		as->maxsize = reqsize + ALLOCATION_CHUNK_SIZE;
		as->text = (char *)xrealloc(as->text, as->maxsize + 1);
	}
}

astr astr_new(void)
{
	astr as;
	as = (astr)xmalloc(sizeof *as);
	as->maxsize = ALLOCATION_CHUNK_SIZE;
	as->size = 0;
	as->text = (char *)xmalloc(as->maxsize + 1);
	memset(as->text, 0, as->maxsize + 1);
	return as;
}

astr astr_copy(castr as)
{
	astr dest;
	assert(as != NULL);
	dest = astr_new();
	astr_assign(dest, as);
	return dest;
}

astr astr_copy_cstr(const char *s)
{
	astr dest;
	assert(s != NULL);
	dest = astr_new();
	astr_assign_cstr(dest, s);
	return dest;
}

void astr_delete(astr as)
{
	assert(as != NULL);
	free(as->text);
	as->text = NULL;
	free(as);
}

void astr_clear(astr as)
{
	assert(as != NULL);
	as->text[0] = '\0';
	as->size = 0;
}

char *(astr_cstr)(castr as)
{
	assert(as != NULL);
	return as->text;
}

size_t (astr_size)(castr as)
{
	assert(as != NULL);
	return as->size;
}

astr astr_fill(astr as, int c, size_t size)
{
	assert(as != NULL);
	resize_if_smaller(as, size);
	memset(as->text, c, size);
	as->text[size] = '\0';
	return as;
}

int (astr_cmp)(castr s1, castr s2)
{
	assert(s1 != NULL && s2 != NULL);
	return strcmp(s1->text, s2->text);
}

int (astr_cmp_cstr)(castr s1, const char *s2)
{
	assert(s1 != NULL && s2 != NULL);
	return strcmp(s1->text, s2);
}

int (astr_eq)(castr s1, castr s2)
{
	assert(s1 != NULL && s2 != NULL);
	return !strcmp(s1->text, s2->text);
}

int (astr_eq_cstr)(castr s1, const char *s2)
{
	assert(s1 != NULL && s2 != NULL);
	return !strcmp(s1->text, s2);
}

static astr astr_assign_x(astr as, const char *s, size_t csize)
{
	resize_if_smaller(as, csize);
	strcpy(as->text, s);
	as->size = csize;
	return as;
}

astr astr_assign(astr as, castr src)
{
	assert(as != NULL && src != NULL);
	return astr_assign_x(as, src->text, src->size);
}

astr astr_assign_cstr(astr as, const char *s)
{
	assert(as != NULL && s != NULL);
	return astr_assign_x(as, s, strlen(s));
}

astr astr_assign_char(astr as, int c)
{
	assert(as != NULL);
	as->text[0] = c;
	as->text[1] = '\0';
	as->size = 1;

	return as;
}

static astr astr_insert_x(astr as, int pos, const char *s, size_t csize)
{
	astr dest = astr_new();
	if (pos < 0) {
		pos = as->size + pos;
		if (pos < 0)
			pos = 0;
	}
	if ((unsigned int)pos > as->size)
		pos = as->size;
	dest->size = as->size + csize;
	resize_if_smaller(dest, dest->size);
	memcpy(dest->text, as->text, pos);
	memcpy(dest->text + pos, s, csize);
	strcpy(dest->text + pos + csize, as->text + pos);
	free(as->text);
	*as = *dest;
	free(dest);
	return as;
}

astr astr_insert(astr as, int pos, castr src)
{
	assert(as != NULL && src != NULL);
	return astr_insert_x(as, pos, src->text, src->size);
}

astr astr_insert_cstr(astr as, int pos, const char *s)
{
	assert(as != NULL && s != NULL);
	return astr_insert_x(as, pos, s, strlen(s));
}

astr astr_insert_char(astr as, int pos, int c)
{
	char buf[2] = {c, '\0'};
	assert(as != NULL);
	return astr_insert_x(as, pos, buf, 1);
}

astr astr_prepend(astr as, castr src)
{
	return astr_insert(as, 0, src);
}

astr astr_prepend_cstr(astr as, const char *s)
{
	return astr_insert_cstr(as, 0, s);
}

astr astr_prepend_char(astr as, int c)
{
	return astr_insert_char(as, 0, c);
}

static astr astr_append_x(astr as, const char *s, size_t csize)
{
	resize_if_smaller(as, as->size + csize);
	strcpy(as->text + as->size, s);
	as->size += csize;
	return as;
}

astr astr_append(astr as, castr src)
{
	assert(as != NULL && src != NULL);
	return astr_append_x(as, src->text, src->size);
}

astr astr_append_cstr(astr as, const char *s)
{
	assert(as != NULL && s != NULL);
	return astr_append_x(as, s, strlen(s));
}

astr astr_append_char(astr as, int c)
{
	assert(as != NULL);
	resize_if_smaller(as, as->size + 1);
	as->text[as->size] = c;
	as->text[++as->size] = '\0';

	return as;
}

astr astr_remove(astr as, int pos, size_t size)
{
	assert(as != NULL);
	if (pos < 0) {
		pos = as->size + pos;
		if (pos < 0)
			pos = 0;
	}
	if ((unsigned int)pos > as->size)
		pos = as->size;

	if (as->size - pos < size)
		size = as->size - pos;
	if (size > 0) {
		strcpy(as->text + pos, as->text + pos + size);
		as->size -= size;
	}
	return as;
}

astr astr_truncate(astr as, size_t size)
{
	assert(as != NULL);
	if (size < as->size) {
		as->size = size;
		as->text[size] = '\0';
	}

	return as;
}

astr astr_substr(castr as, int pos, size_t size)
{
	astr dest;
	assert(as != NULL);
	dest = astr_new();
	if (pos < 0) {
		pos = as->size + pos;
		if (pos < 0)
			pos = 0;
	}
	if ((unsigned int)pos > as->size)
		pos = as->size;

	if (as->size - pos < size)
		size = as->size - pos;
	if (size > 0) {
		resize_if_smaller(dest, size);
		memcpy(dest->text, as->text + pos, size);
		dest->size = size;
	}
	return dest;
}

astr astr_left(castr as, size_t size)
{
	assert(as != NULL);
	if (size == 0)
		return astr_new();
	if (size > as->size)
		size = as->size;
	return astr_substr(as, 0, size);
}

astr astr_right(castr as, size_t size)
{
	assert(as != NULL);
	if (size == 0)
		return astr_new();
	if (size > as->size)
		size = as->size;
	return astr_substr(as, as->size - size, size);
}

int astr_find(castr as, castr src)
{
	return astr_find_cstr(as, src->text);
}

int astr_find_cstr(castr as, const char *s)
{
	char *sp;
	assert(as != NULL && s != NULL);
	sp = strstr(as->text, s);
	return (sp == NULL) ? -1 : sp - as->text;
}

int astr_find_char(castr as, int c)
{
	char *sp;
	assert(as != NULL);
	sp = strchr(as->text, c);
	return (sp == NULL) ? -1 : sp - as->text;
}

int astr_rfind(castr as, castr src)
{
	return astr_rfind_cstr(as, src->text);
}

int astr_rfind_cstr(castr as, const char *s)
{
	char *sp, *prevsp;
	assert(as != NULL && s != NULL);
	prevsp = NULL;
	sp = as->text;
	while ((sp = strstr(sp, s)) != NULL)
		prevsp = sp;
	return (prevsp == NULL) ? -1 : prevsp - as->text;
}

int astr_rfind_char(castr as, int c)
{
	char *sp;
	assert(as != NULL);
	sp = strrchr(as->text, c);
	return (sp == NULL) ? -1 : sp - as->text;
}

static astr astr_replace_x(astr as, int pos, size_t size, const char *s, size_t csize)
{
	astr dest = astr_new();
	if (pos < 0) {
		pos = as->size + pos;
		if (pos < 0)
			pos = 0;
	}
	if ((unsigned int)pos > as->size)
		pos = as->size;

	if (as->size - pos < size)
		size = as->size - pos;
	if (size > 0) {
		dest->size = as->size - size + csize;
		resize_if_smaller(dest, dest->size);
		memcpy(dest->text, as->text, pos);
		memcpy(dest->text + pos, s, csize);
		strcpy(dest->text + pos + csize, as->text + pos + size);
	}
	free(as->text);
	*as = *dest;
	free(dest);
	return as;
}

astr astr_replace(astr as, int pos, size_t size, castr src)
{
	assert(as != NULL && src != NULL);
	return astr_replace_x(as, pos, size, src->text, src->size);
}

astr astr_replace_cstr(astr as, int pos, size_t size, const char *s)
{
	assert(as != NULL && s != NULL);
	return astr_replace_x(as, pos, size, s, strlen(s));
}

astr astr_replace_char(astr as, int pos, size_t size, int c)
{
	char buf[2] = {c, '\0'};
	assert(as != NULL);
	return astr_replace_x(as, pos, size, buf, 1);
}

astr astr_fgets(astr as, FILE *f)
{
	int c;
	assert(as != NULL);
	astr_clear(as);
	if ((c = fgetc(f)) == EOF)
		return NULL;
	astr_append_char(as, c);
	while ((c = fgetc(f)) != EOF && c != '\n')
		astr_append_char(as, c);
	return as;
}

void astr_fputs(castr as, FILE *f)
{
	fputs(as->text, f);
}

astr astr_fmt(astr as, const char *fmt, ...)
{
	va_list ap;
#ifdef HAVE_VASPRINTF
	char *buf;
#else
	char buf[2048]; /* XXX fix this */
#endif
	va_start(ap, fmt);
#ifdef HAVE_VASPRINTF
	vasprintf(&buf, fmt, ap);
#else
	vsprintf(buf, fmt, ap);
#endif
	va_end(ap);
	astr_assign_cstr(as, buf);
#ifdef HAVE_VASPRINTF
	free(buf);
#endif
	return as;
}

astr astr_afmt(astr as, const char *fmt, ...)
{
	va_list ap;
#ifdef HAVE_VASPRINTF
	char *buf;
#else
	char buf[2048]; /* XXX fix this */
#endif
	va_start(ap, fmt);
#ifdef HAVE_VASPRINTF
	vasprintf(&buf, fmt, ap);
#else
	vsprintf(buf, fmt, ap);
#endif
	va_end(ap);
	astr_append_cstr(as, buf);
#ifdef HAVE_VASPRINTF
	free(buf);
#endif
	return as;
}

#ifdef TEST

void assert_eq(astr as, const char *s)
{
	if (!astr_eq_cstr(as, s))
		printf("test failed: \"%s\" != \"%s\"\n", as->text, s);
}

int main(void)
{
	astr as1, as2, as3;
	int i;

	as1 = astr_new();
	astr_assign_cstr(as1, " world");
	astr_prepend_cstr(as1, "hello");
	astr_append_char(as1, '!');
	assert_eq(as1, "hello world!");

	as3 = astr_substr(as1, 6, 5);
	assert_eq(as3, "world");

	as2 = astr_new();
	astr_assign_cstr(as2, " ");
	astr_prepend_cstr(as2, "The");
	astr_append(as2, as3);
	astr_append_char(as2, '.');
	assert_eq(as2, "The world.");

	astr_delete(as3);
	as3 = astr_substr(as1, -6, 5);
	assert_eq(as3, "world");

	astr_assign_cstr(as1, "12345");
	astr_delete(as2);
	as2 = astr_left(as1, 10);
	assert_eq(as2, "12345");
	astr_delete(as2);
	as2 = astr_left(as1, 3);
	assert_eq(as2, "123");
	astr_delete(as2);
	as2 = astr_right(as1, 10);
	assert_eq(as2, "12345");
	astr_delete(as2);
	as2 = astr_right(as1, 3);
	assert_eq(as2, "345");

	astr_assign_cstr(as1, "12345");
	astr_insert_cstr(as1, 3, "mid");
	astr_insert_cstr(as1, 0, "begin");
	astr_insert_cstr(as1, 100, "end");
	assert_eq(as1, "begin123mid45end");

	astr_assign_cstr(as1, "12345");
	astr_insert_char(as1, -2, 'x');
	astr_insert_char(as1, -10, 'y');
	astr_insert_char(as1, 10, 'z');
	assert_eq(as1, "y123x45z");

	astr_assign_cstr(as1, "12345");
	astr_delete(as2);
	as2 = astr_substr(as1, -2, 5);
	assert_eq(as2, "45");

	astr_assign_cstr(as1, "12345");
	astr_delete(as2);
	as2 = astr_substr(as1, -10, 5);
	assert_eq(as2, "12345");

	astr_assign_cstr(as1, "1234567");
	astr_replace_cstr(as1, -4, 2, "foo");
	assert_eq(as1, "123foo67");

	astr_assign_cstr(as1, "1234567");
	astr_replace_cstr(as1, 1, 3, "foo");
	assert_eq(as1, "1foo567");

	astr_assign_cstr(as1, "1234567");
	astr_replace_cstr(as1, -1, 5, "foo");
	assert_eq(as1, "123456foo");

	astr_assign_cstr(as1, "1234567");
	astr_remove(as1, 4, 10);
	assert_eq(as1, "1234");

	astr_assign_cstr(as1, "abc def de ab cd ab de fg");
	while ((i = astr_find_cstr(as1, "de")) >= 0)
	       astr_replace_cstr(as1, i, 2, "xxx");
	assert_eq(as1, "abc xxxf xxx ab cd ab xxx fg");
	while ((i = astr_find_cstr(as1, "ab")) >= 0)
	       astr_remove(as1, i, 2);
	assert_eq(as1, "c xxxf xxx  cd  xxx fg");
	while ((i = astr_find_cstr(as1, "  ")) >= 0)
	       astr_replace_char(as1, i, 2, ' ');
	assert_eq(as1, "c xxxf xxx cd xxx fg");

	astr_fill(as1, 'x', 3);
	assert_eq(as1, "xxx");

	astr_fmt(as1, "%s * %d = ", "5", 3);
	astr_afmt(as1, "%d", 15);
	assert_eq(as1, "5 * 3 = 15");

	printf("Input one string: ");
	fflush(stdout);
	astr_fgets(as1, stdin);
	printf("You wrote: \"%s\"\n", astr_cstr(as1));

	astr_delete(as1);
	astr_delete(as2);
	astr_delete(as3);
	printf("astr test successful.\n");

	return 0;
}

#endif /* TEST */
