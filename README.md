#OS200 Assignment

My OS200 Assignment from Semester 1 2015.

The task was to simply write 2 versions of a simple program that will add up a provided list of integers across a provided number of processes / threads.

##Build
Build using the provided Make files on linux systems (tested on Redhat 6.6)

##Usage

    ./Adder [FILE] [K]

File - the file of comma seperated integers to add together.
K - The specified number of processes / threads that will preform the additions. Each process / thread will read an equal-ish amnount of integers, add them together, then the parent will read them nd add to the running total atomically.

A test input file has been included.

