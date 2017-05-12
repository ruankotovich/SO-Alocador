#!/bin/bash

./cheeki < workloadMem0 > saida$1
./cheeki < workloadMem1 >> saida$1
./cheeki < workloadMem2 >> saida$1
