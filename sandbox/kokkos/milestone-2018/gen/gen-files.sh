#!/bin/bash


for loop_count in 1 2 3 4 5 10 15 30 60 120
do
    ./kokkos-gen-src.py $loop_count 
done

		  
