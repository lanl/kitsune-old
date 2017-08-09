#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

unsigned long long todval (struct timeval *tp) {
    return tp->tv_sec * 1000 * 1000 + tp->tv_usec;
}

int ok (int n, char *a) {

  int i, j;
  char p, q;

  for (i = 0; i < n; i++) {
    p = a[i];
    for (j = i + 1; j < n; j++) {
      q = a[j];
      if (q == p || q == p - (j - i) || q == p + (j - i))
        return 0;
    }
  }

  return 1;
}

int nqueens (int n, int j, char *a) {
  char* b; 
  int i;
  int count[n];
  int solNum = 0;

  if (n == j) 
    return 1;

  memset(count, 0, n * sizeof (int));

  for (i = 0; i < n; i++) {
    b = (char *) alloca((j + 1) * sizeof (char));
    memcpy(b, a, j * sizeof (char));
    b[j] = i;

    if(ok (j + 1, b)) {
      #pragma omp task shared(count)
      count[i] = nqueens(n, j + 1, b);  
    }
  }
  #pragma omp taskwait

  for(i = 0; i < n; i++) 
    solNum += count[i];

  return solNum;
}


int main(int argc, char *argv[]) { 

  int res, n = argc > 1 ? atoi(argv[1]) : 13; 
  char *a = (char *) alloca (n * sizeof (char));

  #pragma omp parallel 
  #pragma omp single nowait
  res = nqueens(n, 0, a);

  printf("nqueens(%d) = %d\n", n, res);
}
