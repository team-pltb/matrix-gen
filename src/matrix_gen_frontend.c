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
#include <string.h>
#include <stdbool.h>

#include "matrix_gen_bf.h"
#include "matrix_gen_ind.h"

static int cmp_matrix(const void *e1, const void *e2){
	matrix_t *m1 = (matrix_t*)e1;
	matrix_t *m2 = (matrix_t*)e2;
	int result = (int) (m1->free_parameters - m2->free_parameters);
	if (result == 0) {
		result = strncmp(&m1->entries[0], &m2->entries[0], MATRIX_LENGTH);
	}
	return result;
}

int main(int argc, char *argv[])
{
	matrix_t matrices[203];

	if (argc != 2) {
                printf("Usage: ./matrix_gen (-bf|-ind)\n-bf use bruteforce approach\n-ind use inductive approach\n");
		return 1;
	}

	char *cmd = argv[1];

	if (strcmp(cmd, "-bf") == 0
		|| strcmp(cmd, "-ind") == 0) {
		if (strcmp(cmd, "-bf") == 0) {
			generate_matrices_brute_force(&matrices[0]);
		} else if (strcmp(cmd, "-ind") == 0) {
			generate_matrices_inductive(&matrices[0]);
		}
		qsort(&matrices[0], TOTAL_MATRICES, sizeof(matrix_t), cmp_matrix);
		for (int i = 0; i < TOTAL_MATRICES; i++) {
			printf("%03d: %.*s (%d)\n", i, MATRIX_LENGTH, &matrices[i].entries[0], matrices[i].free_parameters);
		}
	}
}
