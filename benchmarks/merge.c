#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
void merge (int *a, int *b, int* tmp, int n) {
  int i, *p, *q;
  for (i = 0, p = a, q = b; i < n; i++) 
    tmp[i] = p == b     ? *q++       /* exhausted list a */
           : q == a + n ? *p++       /* exhausted list b */
           : *p < *q    ? *p++       /* a  < b */
           :              *q++;      /* a !< b */
  memcpy(a, tmp, n * sizeof (int)); /* move temporary into results */
}   

void merge_sort (int *a, int* ta, int n) {
  int m;
  if (n < 2)
    return;
  m = n / 2;
  #pragma omp task
  merge_sort(a, ta, m);
  #pragma omp task
  merge_sort(a+m, ta+m, n - m);
  #pragma omp taskwait
  merge(a, a+m, ta, n);
}

int main(int argc, char **argv) {
  int i, n = argc > 1 ? atoi(argv[1]) : 10000000; 
  int print = argc > 2 ? atoi(argv[2]) : 0;
  int *lst = malloc(sizeof(int) * 2 * n);
  int *tmp = lst + n; 
  for(i=0;i<n;i++)
    lst[i] = rand();
  #pragma omp parallel 
  #pragma omp single nowait
  merge_sort(lst, tmp, n);
  for(i=0; i<n-1; i++)
    if(lst[i] > lst[i+1]){
      printf("not sorted!, lst[%d] > lst[%d]\n", i, i+1); 
      return 1;
    }
  printf("sorted succesfully\n"); 
  return 0;
}

