#ifdef LOOP_NEST
Kokkos::parallel_for (SIZE, KOKKOS_LAMBDA (const int i) {
  Kokkos::parallel_for (SIZE, KOKKOS_LAMBDA (const int j) {
    a(i, j, 0) += i * 100000.0 + j * 10000.0 + 0;
    a(i, j, 1) += i * 100000.0 + j * 10000.0 + 1;
  });
});
#else
Kokkos::parallel_for (SIZE, KOKKOS_LAMBDA (const int i) {
  a(i, 0) += i * 100.0;
  a(i, 1) += i * 10000.0;
});
#endif