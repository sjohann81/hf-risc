#!/bin/bash
make clean
make tp1
../tools/sim/hf_riscv_sim/hf_riscv_sim code.bin
