#include "BasicLevenshteinDistance.h"
#include <string.h>
#include <stdlib.h> 
//#include <string.h>

//****************************
// Get minimum of three values
//****************************

int BasicLevenshteinDistance::minimum (int a, int b, int c) {
  int mi;
  mi = a;
  if (b < mi) {
    mi = b;
  }
  if (c < mi) {
    mi = c;
  }
  return mi;
}

//**************************************************
// Get a pointer to the specified cell of the matrix
//**************************************************

int *BasicLevenshteinDistance::getCellPointer (int *pOrigin, int col, int row, int nCols)
{
  return pOrigin + col + (row * (nCols + 1));
}

//*****************************************************
// Get the contents of the specified cell in the matrix
//*****************************************************

int BasicLevenshteinDistance::getAt (int *pOrigin, int col, int row, int nCols) {

int *pCell;

  pCell = getCellPointer (pOrigin, col, row, nCols);
  return *pCell;

}

//*******************************************************
// Fill the specified cell in the matrix with the value x
//*******************************************************

void BasicLevenshteinDistance::putAt (int *pOrigin, int col, int row, int nCols, int x)
{
int *pCell;

  pCell = getCellPointer (pOrigin, col, row, nCols);
  *pCell = x;

}

//*****************************
// Compute Levenshtein distance
//*****************************

int BasicLevenshteinDistance::calcualteDistance(char const *s, char const *t)
{
int *d; // pointer to matrix
int n; // length of s
int m; // length of t
int i; // iterates through s
int j; // iterates through t
char s_i; // ith character of s
char t_j; // jth character of t
int cost; // cost
int result; // result
int cell; // contents of target cell
int above; // contents of cell immediately above
int left; // contents of cell immediately to left
int diag; // contents of cell immediately above and to left
int sz; // number of cells in matrix

  // Step 1

  n = (int) strlen (s);
  m = (int) strlen (t);
  if (n == 0) {
    return m;
  }
  if (m == 0) {
    return n;
  }
  sz = (n+1) * (m+1) * sizeof (int);
  d = (int *) malloc ((size_t) sz);

  // Step 2

  for (i = 0; i <= n; i++) {
    putAt (d, i, 0, n, i);
  }

  for (j = 0; j <= m; j++) {
    putAt (d, 0, j, n, j);
  }

  // Step 3

  for (i = 1; i <= n; i++) {

    s_i = s[i-1];

    // Step 4

    for (j = 1; j <= m; j++) {

      t_j = t[j-1];

      // Step 5

      if (s_i == t_j) {
        cost = 0;
      }
      else {
        cost = 1;
      }

      // Step 6

      above = getAt (d,i-1,j, n);
      left = getAt (d,i, j-1, n);
      diag = getAt (d, i-1,j-1, n);
      cell = minimum (above + 1, left + 1, diag + cost);
      putAt (d, i, j, n, cell);
    }
  }

  // Step 7

  result = getAt (d, n, m, n);
  free (d);
  return result;

}
