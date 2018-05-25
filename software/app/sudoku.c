#include <hf-risc.h>

void print_puzzle(char (*a)[9], int l, int c)
{
	int i = 0, j;

	while (i < l){
		j = 0;
		while (j < c){
			printf("%d ", a[i][j]);
			j++;
		}
		i++;
		printf("\n");
	}
}

int is_valid(char number, char (*puzzle)[9], int row, int column)
{
	int i = 0;
	int sector_row = 3 * (row / 3), sector_col = 3 * (column / 3), row1 = (row + 2) % 3,
		row2 = (row + 4) % 3, col1 = (column + 2) % 3, col2 = (column + 4) % 3;

	while (i < 9){
		if (puzzle[i][column] == number) return 0;
		if (puzzle[row][i] == number) return 0;
		i++;
	}

	if (puzzle[row1 + sector_row][col1 + sector_col] == number) return 0;
	if (puzzle[row2 + sector_row][col1 + sector_col] == number) return 0;
	if (puzzle[row1 + sector_row][col2 + sector_col] == number) return 0;
	if (puzzle[row2 + sector_row][col2 + sector_col] == number) return 0;

	return 1;
}

int sudoku(char (*puzzle)[9], int row, int column)
{
	int nextn = 1;

	if (row == 9)
		return 1;

	if (puzzle[row][column]) {
		if (column == 8) {
			if (sudoku(puzzle, row + 1, 0)) return 1;
		} else {
			if (sudoku(puzzle, row, column + 1)) return 1;
		}
		return 0;
	}

	while (nextn < 10) {
		if (is_valid(nextn, puzzle, row, column)) {
			puzzle[row][column] = nextn;
			if (column == 8) {
				if (sudoku(puzzle, row + 1, 0)) return 1;
			} else {
				if (sudoku(puzzle, row, column + 1)) return 1;
			}
			puzzle[row][column] = 0;
		}
		nextn++;
	}

	return 0;
}

int main()
{
	char puzzle[9][9] = {
		{0, 0, 0, 9, 0, 4, 0, 0, 1},
		{0, 2, 0, 3, 0, 0, 0, 5, 0},
		{9, 0, 6, 0, 0, 0, 0, 0, 0},
		{8, 0, 0, 0, 4, 6, 0, 0, 0},
		{4, 0, 0, 0, 1, 0, 0, 0, 3},
		{0, 0, 0, 2, 7, 0, 0, 0, 5},
		{0, 0, 0, 0, 0, 0, 9, 0, 7},
		{0, 7, 0, 0, 0, 5, 0, 1, 0},
		{3, 0, 0, 4, 0, 7, 0, 0, 0}
	};


	puts("\npuzzle 1\n");
	print_puzzle(puzzle, 9, 9);
	puts("\nsolving ...\n");
	sudoku(puzzle, 0, 0);
	print_puzzle(puzzle, 9, 9);

	return 0;
}
