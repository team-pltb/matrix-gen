/**
 * This file is part of PLTB.
 * Copyright (C) 2015 Michael Hoff, Stefan Orf and Benedikt Riehm
 *
 * PLTB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PLTB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PLTB.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "matrix_gen_ind.h"

#define BASE_CHAR '0'

#define DEBUG 0

#if DEBUG
#define DBG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DBG(fmt, ...)
#endif

typedef struct {
	matrix_t *matrix;
	unsigned promotion_index;
} pmatrix_t;

typedef struct {
	pmatrix_t **elements;
	unsigned curr_index;
} pmatrix_queue_t;

#if DEBUG
static int size(pmatrix_queue_t *queue) {
	return queue->curr_index;
}
#endif

static bool has(pmatrix_queue_t *queue) {
	return queue->curr_index > 0;
}

static pmatrix_t *pop(pmatrix_queue_t *queue) {
	if (has(queue)) {
		return queue->elements[--queue->curr_index];
	}
	return NULL;
}

static void push(pmatrix_queue_t *queue, pmatrix_t *matrix) {
	assert(queue->curr_index < TOTAL_MATRICES);
	queue->elements[queue->curr_index++] = matrix;
}

static pmatrix_queue_t *new_queue(unsigned size) {
	pmatrix_queue_t *queue = (pmatrix_queue_t*)malloc(sizeof(pmatrix_queue_t));
	queue->elements = (pmatrix_t**)malloc(sizeof(pmatrix_t*) * size);
	return queue;
}

#if DEBUG
static void println_pmatrix(char *prefix, pmatrix_t *pmatrix, int index) {
	printf("%s", prefix);
	for (unsigned i = 0; i < MATRIX_LENGTH; i++) {
		if (pmatrix->promotion_index == i) {
			printf("|");
		}
		printf("%c", pmatrix->matrix->entries[i]);
	}
	if (pmatrix->promotion_index == MATRIX_LENGTH) {
		printf("|");
	}
	printf(" (%u) [%d]\n", pmatrix->matrix->free_parameters, index);
}
#endif

static pmatrix_t *spawn(pmatrix_t *base_pmatrix, pmatrix_t *new_pmatrix) {
	new_pmatrix->promotion_index = base_pmatrix->promotion_index;
	memcpy(&new_pmatrix->matrix->entries, &base_pmatrix->matrix->entries, sizeof(char) * MATRIX_LENGTH);
	return new_pmatrix;
}

static void promote(pmatrix_t *base_pmatrix, int start,
		pmatrix_queue_t *pool, pmatrix_queue_t *next,
		char prev_char, char next_char, bool inc) {
	for (unsigned i = (unsigned int) start; i < MATRIX_LENGTH; i++) {
		if (base_pmatrix->matrix->entries[i] == prev_char) {
			// promotion possible
			pmatrix_t *curr_pmatrix = spawn(base_pmatrix, pop(pool));
			curr_pmatrix->matrix->entries[i] = next_char;
			curr_pmatrix->matrix->free_parameters = (unsigned int) (next_char - BASE_CHAR + 1);
			if (inc) {
				curr_pmatrix->promotion_index = i + 1;
				/* we derive and protect the promoted character */
#if DEBUG
				println_pmatrix("=> ", curr_pmatrix, TOTAL_MATRICES - pool->curr_index);
#endif
			} else {
				/* we derive and allow subsequent promotions to affect this character */
#if DEBUG
				println_pmatrix("-> ", curr_pmatrix, TOTAL_MATRICES - pool->curr_index);
#endif
			}
			/* we must save the derived matrices for the next K */
			push(next, curr_pmatrix);
			/* this recursion will _NOT_ affect already executed promotions (protected by i + 1) */
			promote(curr_pmatrix, (int) (i + 1), pool, next, prev_char, next_char, false);
		}
	}
}

void generate_matrices_inductive(matrix_t *matrices)
{
	pmatrix_t pmatrices[TOTAL_MATRICES];
	for (unsigned i = 0; i < TOTAL_MATRICES; i++) {
		pmatrices[i].matrix = &matrices[i];
	}

	/* pool_queue holds slots for all matrices to be calculated */
	pmatrix_queue_t *pool_queue = new_queue(TOTAL_MATRICES);
	for (int i = TOTAL_MATRICES - 1; i >= 0; i--) {
		/* push in reverse order to pop in right order */
		push(pool_queue, pmatrices + i);
	}
	/* curr_queue holds the matrices of the last K */
	pmatrix_queue_t *curr_queue = new_queue(TOTAL_MATRICES);
	/* next_queue buffers the matrices of the next K */
	pmatrix_queue_t *next_queue = new_queue(TOTAL_MATRICES);

	char prev_char = BASE_CHAR;

	pmatrix_t *first_pmatrix = pop(pool_queue);

	/* * * *
	 * Build the first matrix by hand.
	 * K = 1
	 * * * */
	for (unsigned i = 0; i < MATRIX_LENGTH; i++) {
		first_pmatrix->matrix->entries[i] = prev_char;
	}
	/* we may not change the leading 0 */
	first_pmatrix->promotion_index = 1;
	first_pmatrix->matrix->free_parameters = 1;

	/* our only matrix in the first K */
	push(curr_queue, first_pmatrix);

	/* K = 2 .. MATRIX_LENGTH */
	for (unsigned K = 2; K <= MATRIX_LENGTH; K++) {
		/* we will promote prev_char through next_char in this K */
		char curr_char = prev_char + 1;
		DBG("K = %u, %c -> %c\n", K, prev_char, curr_char);
		while (has(curr_queue)) {
			/* for every matrix of the last K */
			pmatrix_t *base_pmatrix = pop(curr_queue);
#if DEBUG
			println_pmatrix("B: ", base_pmatrix, 0);
#endif
			/* use base_matrix to derive matrices of the next K
			 * the first character promotion changes the promotionIndex (inc = true)
			 * all subsequent promotions (later Ks) are not required to be final
			 * hence the promotionIndex protects the first character occurence and therefore
			 * avoids semantically equals symmetry matrices.
			 * */
			promote(base_pmatrix, (int) (base_pmatrix->promotion_index), pool_queue, next_queue, prev_char, curr_char, true);
		}
		DBG("Used: %d\n", TOTAL_MATRICES - size(pool_queue));
		/* we cannot proceed */
		if (!has(next_queue)) break;

		/* swap queues */
		pmatrix_queue_t *tmp = curr_queue;
		curr_queue = next_queue;
		next_queue = tmp;

		prev_char = curr_char;
	}
}
