/*
tvm - tencode.c
The Transterpreter - a portable virtual machine for Transputer bytecode
Copyright (C) 2008 Carl G. Ritson

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "tvm.h"

typedef struct _element_t {
	char		id[5];
	UWORD		length;
	union {
		WORD	s_int;
		UWORD	u_int;
		char	*str;
		BYTE	*bytes;
	} data;
	BYTE		*next;
} element_t;

typedef struct _tenc_str_t tenc_str_t;
struct _tenc_str_t {
	tenc_str_t	*next;
	char		*str;
};

typedef struct _tbc_tlp_t {
	char		*fmt;
	char		*desc;
} tbc_tlp_t;

typedef struct _tbc_ffi_t {
	tenc_str_t 	*libraries;
	tenc_str_t	*symbols;
	unsigned int	map_len;
	UWORD		*map;
} tbc_ffi_t;

typedef struct _tbc_lni_entry_t tbc_lni_entry_t;
struct _tbc_lni_entry_t {
	tbc_lni_entry_t		*next;
	unsigned int 		offset;
	char			*file;
	unsigned int		line;
};

typedef struct _tbc_lni_t {
	tenc_str_t	*files;
	tbc_lni_entry	*data;
} tbc_lni_t;

typedef struct _tbc_t {
	unsigned int	endian;		/* 1 */
	unsigned int	ws;		/* 2 */
	unsigned int	vs;		/* 3 */
	unsigned int	ms;		/* 4 */
	
	unsigned int	bytecode_len;	/* 5 */
	BYTE		*bytecode;	/* 6 */

	tbc_tlp_t	*tlp;		/* 7 */

	tbc_ffi_t	*ffi;		/* 8 */
	tbc_lni_t	*line_numbers;	/* 9 */
} tbc_t;

static WORD decode_int (BYTE *src)
{
	WORD value = *((WORD *) src);
	#if defined (TVM_BIG_ENDIAN)
	return value;
	#elif TVM_WORD_LENGTH == 4
	return SwapFourBytes (value);
	#elif TVM_WORD_LENGTH == 2
	return SwapTwoBytes (value);
	#else
	#error "Unknown TVM_WORD_LENGTH"
	#endif
}

static int decode_element (BYTE *src, int *length, element_t *element)
{
	if (*length < (sizeof (WORD) + 4)) {
		return -1;
	}

	memcpy (element->id, src, 4);
	element->id[4] 	= '\0';
	element->length = decode_int (src + 4);
	element->next	= src + (sizeof (WORD) + 4);
	*length		-= (sizeof (WORD) + 4);

	if (element->id[3] == 'I' || element->id[3] == 'U') {
		element->data.u_int	= element->length;
		element->length		= 0;
		return 0;
	} else if (*length < element->length) {
		return -1;
	} else {
		int bytes = element->length;

		bytes += ((sizeof (WORD)) - (bytes & (sizeof (WORD) - 1)));
		
		element->data.bytes 	= element->next;
		element->next		+= bytes;
		*length			-= bytes;

		return 0;
	}
}

static int walk_to_element (BYTE *data, int *length, const char *id, element_t *element) {
	while (*length > 0) {
		int ret = decode_element (data, length, element);
		int i;

		if (ret < 0) {
			return ret;
		}

		for (i = 0; i < 4; ++i) {
			if (element->id[i] != id[i])
				break;
		}

		if (i == 4) {
			return 0;
		}
		
		data = element->next;
	}

	return -1;
}

static int load_uint (BYTE **data, int *length, const char *id, UWORD *dst)
{
	element_t element;
	int ret;

	if ((ret = walk_to_element (*data, length, id, &element)) < 0)
		return ret;
	
	*dst 	= element.data.u_int;
	*data	= element->next;

	return 0;
}

int tencode_tbc_decode (BYTE *data, int length, tbc_t **ptr)
{
	element_t 	element;
	tbc_t 		*tbc	= (tbc_t *) data;
	int		ret;

	if ((ret = load_uint (&data, &length, "endU", &(tbc->endian))) < 0)
		return ret;
	if ((ret = load_uint (&data, &length, "ws U", &(tbc->ws))) < 0)
		return ret;
	if ((ret = load_uint (&data, &length, "vs U", &(tbc->vs))) < 0)
		return ret;
	if ((ret = load_uint (&data, &length, "ms U", &(tbc->ms))) < 0)
		return ret;

	if ((ret = walk_to_element (data, &length, "bc B", &element)) < 0)
		return ret;
	
	tbc->bytecode_len	= element->length;
	tbc->bytecode		= element->data.bytes;
	data			= element->next;

}

