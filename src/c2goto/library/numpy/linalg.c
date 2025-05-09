// NumPy documentation: https://numpy.org/doc/stable/reference/routines.linalg.html

void dot(const int *A, const int *B, int *C, int m, int n, int n2, int p)
{
  int i = 0;
  int j = 0;
  int k = 0;

  while (i < m)
  {
    while (j < p)
    {
      C[i * p + j] = 0;
      while (k < n)
      {
        C[i * p + j] += A[i * n + k] * B[k * p + j];
        k++;
      }
      j++;
    }
    i++;
  }
}
