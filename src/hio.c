/* Extended Module Player
 * Copyright (C) 1996-2016 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "common.h"
#include "hio.h"
#include "mdataio.h"

static long get_size(FILE *f)
{
	long size, pos;

	pos = ftell(f);
	if (pos >= 0) {
		if (fseek(f, 0, SEEK_END) < 0) {
			return -1;
		}
		size = ftell(f);
		if (fseek(f, pos, SEEK_SET) < 0) {
			return -1;
		}
		return size;
	} else {
		return pos;
	}
}

int8 hio_read8s(HIO_HANDLE *h)
{
	int err;
	int8 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read8s(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread8s(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &ret, 1, 1);
		if (bytes_read != 1) {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint8 hio_read8(HIO_HANDLE *h)
{
	int err;
	uint8 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read8(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread8(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &ret, 1, 1);
		if (bytes_read != 1) {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint16 hio_read16l(HIO_HANDLE *h)
{
	int err;
	uint16 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read16l(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread16l(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		uint8 tmp[2];
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &tmp, 1, 2);
		if (bytes_read == 2) {
			ret = tmp[1] << 8 | tmp[0];
		} else {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint16 hio_read16b(HIO_HANDLE *h)
{
	int err;
	uint16 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read16b(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread16b(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		uint8 tmp[2];
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &tmp, 1, 2);
		if (bytes_read == 2) {
			ret = tmp[0] << 8 | tmp[1];
		} else {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint32 hio_read24l(HIO_HANDLE *h)
{
	int err;
	uint32 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read24l(h->handle.file, &err); 
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread24l(h->handle.mem); 
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		uint8 tmp[3];
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &tmp, 1, 3);
		if (bytes_read == 3) {
			ret = tmp[2] << 16 | tmp[1] << 8 | tmp[0];
		} else {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint32 hio_read24b(HIO_HANDLE *h)
{
	int err;
	uint32 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read24b(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread24b(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		uint8 tmp[3];
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &tmp, 1, 3);
		if (bytes_read == 3) {
			ret = tmp[0] << 16 | tmp[1] << 8 | tmp[2];
		} else {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint32 hio_read32l(HIO_HANDLE *h)
{
	int err;
	uint32 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read32l(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread32l(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		uint8 tmp[4];
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &tmp, 1, 4);
		if (bytes_read == 4) {
			ret = tmp[3] << 24 | tmp[2] << 16 | tmp[1] << 8 | tmp[0];
		} else {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

uint32 hio_read32b(HIO_HANDLE *h)
{
	int err;
	uint32 ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = read32b(h->handle.file, &err);
		if (err != 0) {
			h->error = err;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread32b(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		uint8 tmp[4];
		int bytes_read = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, &tmp, 1, 4);
		if (bytes_read == 4) {
			ret = tmp[0] << 24 | tmp[1] << 16 | tmp[2] << 8 | tmp[3];
		} else {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

size_t hio_read(void *buf, size_t size, size_t num, HIO_HANDLE *h)
{
	size_t ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = fread(buf, size, num, h->handle.file);
		if (ret != num) {
			if (ferror(h->handle.file)) {
				h->error = errno;
			} else {
				h->error = feof(h->handle.file) ? EOF : -2;
			}
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread(buf, size, num, h->handle.mem);
		if (ret != num) {
			h->error = errno;
		}
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		ret = (* h->handle.callbacks->read)(h->handle.callbacks->user_data, buf, size, num);
		if (ret != num) {
			h->error = EOF;
		}
		break;
	}
	}

	return ret;
}

int hio_seek(HIO_HANDLE *h, long offset, int whence)
{
	int ret = -1;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = fseek(h->handle.file, offset, whence);
		if (ret < 0) {
			h->error = errno;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mseek(h->handle.mem, offset, whence);
		if (ret < 0) {
			h->error = errno;
		}
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		ret = (* h->handle.callbacks->seek)(h->handle.callbacks->user_data, offset, whence);
		if (ret < 0) {
			h->error = ret;
		}
		break;
	}
	}

	return ret;
}

long hio_tell(HIO_HANDLE *h)
{
	long ret = -1;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = ftell(h->handle.file);
		if (ret < 0) {
			h->error = errno;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mtell(h->handle.mem);
		if (ret < 0) {
			h->error = ret;
		}
		break;
	case HIO_HANDLE_TYPE_CALLBACKS: {
		ret = (* h->handle.callbacks->tell)(h->handle.callbacks->user_data);
		if (ret < 0) {
			h->error = ret;
		}
		break;
	}
	}

	return ret;
}

int hio_eof(HIO_HANDLE *h)
{
	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		return feof(h->handle.file);
	case HIO_HANDLE_TYPE_MEMORY:
		return meof(h->handle.mem);
	case HIO_HANDLE_TYPE_CALLBACKS:
		return (* h->handle.callbacks->eof)(h->handle.callbacks->user_data);
	default:
		return EOF;
	}
}

int hio_error(HIO_HANDLE *h)
{
	int error = h->error;
	h->error = 0;
	return error;
}

HIO_HANDLE *hio_open(void *path, char *mode)
{
	HIO_HANDLE *h;

	h = (HIO_HANDLE *)xmp_malloc(sizeof (HIO_HANDLE));
	if (h == NULL)
		goto err;
	
	h->error = 0;
	h->type = HIO_HANDLE_TYPE_FILE;
	h->handle.file = fopen(path, mode);
	if (h->handle.file == NULL)
		goto err2;

	h->size = get_size(h->handle.file);
	if (h->size < 0)
		goto err3;

	return h;

    err3:
	fclose(h->handle.file);
    err2:
	xmp_free(h);
    err:
	return NULL;
}

HIO_HANDLE *hio_open_mem(void *ptr, long size)
{
	HIO_HANDLE *h;

	h = (HIO_HANDLE *)xmp_malloc(sizeof (HIO_HANDLE));
	if (h == NULL)
		return NULL;
	
	h->error = 0;
	h->type = HIO_HANDLE_TYPE_MEMORY;
	h->handle.mem = mopen(ptr, size);
	h->size = size;

	return h;
}

HIO_HANDLE *hio_open_file(FILE *f)
{
	HIO_HANDLE *h;

	h = (HIO_HANDLE *)xmp_malloc(sizeof (HIO_HANDLE));
	if (h == NULL)
		return NULL;
	
	h->error = 0;
	h->type = HIO_HANDLE_TYPE_FILE;
	h->handle.file = f /*fdopen(fileno(f), "rb")*/;
	h->size = get_size(f);

	return h;
}

HIO_HANDLE *hio_open_callbacks(struct xmp_io_callbacks *cb)
{
	if (cb == NULL) {
		return NULL;
	}
	
	HIO_HANDLE *h;
	h = (HIO_HANDLE *)xmp_malloc(sizeof (HIO_HANDLE));
	if (h == NULL)
		return NULL;

	h->error = 0;
	h->type = HIO_HANDLE_TYPE_CALLBACKS;
	h->handle.callbacks = cb;
	if (cb->size != NULL) {
		h->size = (* cb->size)(cb->user_data);
	} else {
		int error = (* cb->seek)(cb->user_data, 0, SEEK_END);
		if (error) {
			goto fail;
		}
		h->size = (* cb->tell)(cb->user_data);
		if (h->size < 0) {
			goto fail;
		}
		error = (* cb->seek)(cb->user_data, 0, SEEK_SET);
		if (error) {
			goto fail;
		}
	}

	return h;
fail:
	xmp_free(h);
	return NULL;
}

int hio_close(HIO_HANDLE *h)
{
	int ret;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = fclose(h->handle.file);
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mclose(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CALLBACKS:
		ret = 0;
		break;
	default:
		ret = -1;
	}

	xmp_free(h);
	return ret;
}

long hio_size(HIO_HANDLE *h)
{
	return h->size;
}
