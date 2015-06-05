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
#include <stdio.h>
#include <stdlib.h>

#include "matrix_gen_bf.h"

static void normalize(char *entries) {
	int x = 0;
	char mark[6] = {0,0,0,0,0,0};

	int i;
	for (i = 0; i < 6; i++) {
		if (mark[i] == 0) {
			int k = entries[i];
			int j;
			for (j = i; j < 6; j++) {
				if (entries[j] == k && mark[j] == 0) {
					entries[j] = (char) x;
					mark[j]  = 1;
				}
			}
			x++;
		}
	}
}

static int lookup(matrix_t *matrices, unsigned last_idx, char* entries) {
	for (unsigned i = 0; i < last_idx; i++) {
		if (entries[0] == matrices[i].entries[0]
			&& entries[1] == matrices[i].entries[1]
			&& entries[2] == matrices[i].entries[2]
			&& entries[3] == matrices[i].entries[3]
			&& entries[4] == matrices[i].entries[4]
			&& entries[5] == matrices[i].entries[5]
		) {
			return 1;
		}
	}
	return 0;
}

void generate_matrices_brute_force(matrix_t *matrices)
{
	unsigned last_idx = 0;

	int a = 0;
	for(int b = 0; b <= 1; b++) {
		for(int c = 0; c <= 2; c++) {
			for(int d = 0; d <= 3; d++) {
				for(int e = 0; e <= 4; e++) {
					for(int f = 0; f <= 5; f++) {
						matrix_t *matrix = &matrices[last_idx];
						matrix->entries[0] = (char) a;
						matrix->entries[1] = (char) b;
						matrix->entries[2] = (char) c;
						matrix->entries[3] = (char) d;
						matrix->entries[4] = (char) e;
						matrix->entries[5] = (char) f;

						normalize(&matrix->entries[0]);
						if (!lookup(matrices, last_idx, &matrix->entries[0])) {
							last_idx++;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < TOTAL_MATRICES; i++) {
		matrix_t *matrix = &matrices[i];
		int max = 0;
		for (unsigned j = 0; j < MATRIX_LENGTH; j++) {
			if (matrix->entries[j] > max) max = matrix->entries[j];
			matrix->entries[j] += '0';
		}
		matrix->free_parameters = (unsigned int) (max + 1);
	}
}
