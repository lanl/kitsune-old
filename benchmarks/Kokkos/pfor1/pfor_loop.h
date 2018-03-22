Kokkos::parallel_for (SIZE, KOKKOS_LAMBDA (const int i) {
  a(i, 0) += i * 100.0;
  a(i, 1) += i * 10000.0;
});
