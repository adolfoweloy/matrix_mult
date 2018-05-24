#include "matrix.h"

Matrix_t matrix_load_from_file(FILE *file)
{
  size_t i, j;
  double rows, cols, value;
  fscanf(file, "%lf %lf", &rows, &cols);
  Matrix_t matrix = matrix_create(rows, cols);

  while (fscanf(file, "%ld %ld %lf", &i, &j, &value) != EOF)
    matrix_set(matrix, i-1, j-1, value);

  return matrix;
}

Matrix_t matrix_create(size_t rows, size_t cols)
{
  Matrix_t A;
  A.rows = rows;
  A.cols = cols;
  A.items = malloc(cols * rows * sizeof(double));

  for (size_t i = 0; i < rows; i++)
  {
    *(A.items+i) = malloc(cols * sizeof(double));
    for (size_t j = 0; j < cols; j++)
      A.items[i][j] = 0.0;
  }

  return A;
}

Matrix_t matrix_transpose(Matrix_t a)
{
  Matrix_t b = matrix_create(a.cols, a.rows);
  for (size_t j = 0; j < a.cols; j++)
    for (size_t i = 0; i < a.rows; i++)
      matrix_set(b, j, i, a.items[i][j]);
  return b;
}

void matrix_set(Matrix_t a, size_t row, size_t col, double value)
{
  a.items[row][col] = value;
}

Matrix_t matrix_mult(Matrix_t a, Matrix_t b)
{
  Matrix_t result = matrix_create(a.rows, b.cols);

  for (size_t i=0; i<a.rows; i++) {
    for (size_t j=0; j<b.cols; j++) {
      double sum = 0;
      for (size_t k=0; k<b.rows; k++) {
        sum += a.items[i][k] * b.items[k][j];
      }
      result.items[i][j] = sum;
    }
  }
  return result;
}

Matrix_t matrix_mult_t(Matrix_t a, Matrix_t b)
{
  Matrix_t result = matrix_create(a.rows, b.rows);

  for (size_t i=0; i<a.rows; i++) {
    for (size_t j=0; j<b.rows; j++) {
      double sum = 0;
      for (size_t k=0; k<b.cols; k++) {
        sum += a.items[i][k] * b.items[j][k];
      }
      result.items[i][j] = sum;
    }
  }
  return result;
}

bool matrix_mult_valid(Matrix_t a, Matrix_t b)
{
  return a.cols == b.rows;
}

void matrix_print(Matrix_t a)
{
  for (size_t i=0; i<a.rows; i++) {
    for (size_t j=0; j<a.cols; j++) {
      printf("%03lf ", a.items[i][j]);
    }
    printf("\n");
  }
}

void matrix_print_to_file(FILE *output, Matrix_t a)
{
  fprintf(output, "%ld %ld\n", a.rows, a.cols);
  for (size_t i = 0; i < a.rows; i++)
    for (size_t j = 0; j < a.cols; j++)
      if (a.items[i][j] != 0.0)
        fprintf(output, "%ld %ld %lf\n", i+1, j+1, a.items[i][j]);
}

size_t* matrix_sparsity(Matrix_t a)
{
  size_t *result = malloc(a.rows * sizeof(size_t));
  size_t m = 0;
  for(size_t k = 0; k < a.rows; k++)
	*(result+k) = -1;
  size_t i = 0;
  while (i < a.rows)
  {
    size_t j = 0;
    while (j < a.cols)
    {
      if (a.items[i][j] != 0.0)
      {
        i++;
        break;
      }
      else
        j++;

      if (j == a.cols)
        {
          *(result + m) = i;
		  m++;
		  i++;
        }
    }
  }
  return result;
}

Matrix_t sparsed_matrix(Matrix_t a)
{
  size_t sparsed_rows = num_sparse_rows(a);
  size_t z = 0;
  bool flag = true;
  Matrix_t result = matrix_create(a.rows-sparsed_rows, a.cols);
  
  for(size_t i = 0; i < a.rows; i++)
  {
	flag = true;
	for(size_t j = 0; j < sparsed_rows; j++)
	{
	  if(*(matrix_sparsity(a)+j) == i)
		{
		  flag = false;
		  z++;
		}
	}
	if(flag)
	  for(size_t k = 0; k < a.cols; k++)
	  	result.items[i-z][k] = a.items[i][k];
  }
  return result;
}

size_t num_sparse_rows(Matrix_t a)
{
  size_t sparsed_rows = 0;
  for(size_t i = 0; i < a.rows; i++)
    if(*(matrix_sparsity(a)+i) != -1)
	sparsed_rows++; 
  return sparsed_rows;
}

Matrix_t de_sparsing(Matrix_t a, size_t num_sparsed_rows, size_t *sparsed_rows)
{
  size_t z = 0;
  bool flag = true;
  Matrix_t result = matrix_create(a.rows+num_sparsed_rows, a.cols);

  for(size_t i = 0; i < a.rows+num_sparsed_rows; i++)
  {
	flag = true;
	for(size_t j = 0; j < num_sparsed_rows; j++)
	{
	  if(*(sparsed_rows+j) == i)
	  {
		flag = false;
		z++;
	  }
	}
	if(flag)
	  for(size_t k = 0; k < a.cols; k++)
	  	result.items[i][k] = a.items[i-z][k];
  }

  return result;
}
