.PHONY: test
test:
	-[ ! -f influenza.faa ] || [ ! -f protein.fa ] && tar xf input.tar.bz2 && mv input/* ./
	python ../src/read_fasta/read_fasta.py influenza.faa
	python ../src/read_fasta/read_fasta.py protein.fa
	../src/main_compare/build/main.out influenza_seq_css.txt protein_seq_css.txt result.bin


.PHONY: clean
clean:
	-rm -rf input
	-rm influenza.faa
	-rm protein.fa
	-rm -f *.txt
	-rm -f subhash*



