#!/bin/sh
python ../src/read_fasta/read_fasta.py influenza.faa
python ../src/read_fasta/read_fasta.py protein.fa

../src/main_compare/build/main.out influenza_seq_css.txt protein_seq_css.txt result.txt
