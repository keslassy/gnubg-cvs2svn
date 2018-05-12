/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 3 or later of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

#ifndef SIMD_H
#define SIMD_H

#if defined(USE_SIMD_INSTRUCTIONS)

#include <stdlib.h>
#include "common.h"

#if defined(USE_AVX)
#define ALIGN_SIZE 32
#define VEC_SIZE 8
#define LOG2VEC_SIZE 3
#define float_vector __m256
#define int_vector __m256i
#else
#define ALIGN_SIZE 16
#define VEC_SIZE 4
#define LOG2VEC_SIZE 2
#if defined(HAVE_SSE)
#define float_vector __m128
#define int_vector __m128i
#elif defined(HAVE_NEON)
#define float_vector float32x4_t
#define int_vector int32x4_t
#else
#error "Inconsistent SIMD defines"
#endif
#endif

#if defined(_MSC_VER)
#define SSE_ALIGN(D) __declspec(align(ALIGN_SIZE)) D
#else
#define SSE_ALIGN(D) D __attribute__ ((aligned(ALIGN_SIZE)))
#endif

#if defined(__GNUC__) && defined(WIN32)
/* Align stack pointer on 16 byte boundary so SSE variables work correctly */
#define SIMD_STACKALIGN __attribute__((force_align_arg_pointer))
#if defined(USE_AVX)
#define SIMD_AVX_STACKALIGN __attribute__((force_align_arg_pointer))
#else
#define SIMD_AVX_STACKALIGN
#endif                          /* USE_AVX */
#else
#define SIMD_STACKALIGN
#define SIMD_AVX_STACKALIGN
#endif                          /* GNUC/WIN32 */

#define sse_aligned(ar) (!(((size_t)ar) % ALIGN_SIZE))

extern float *sse_malloc(size_t size);
extern void sse_free(float *ptr);

#if defined(HAVE_NEON)
extern int CheckNEON(void);
#endif

#else /* USE_SIMD_INSTRUCTIONS */

#define SSE_ALIGN(D) D
#define sse_malloc malloc
#define sse_free free
#define SIMD_STACKALIGN
#define SIMD_AVX_STACKALIGN

#endif

#endif /* SIMD_H */
