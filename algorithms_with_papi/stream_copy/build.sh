#!/bin/bash

file=$1

gcc -o stream $file -O2 -lpapi -mcmodel=large
