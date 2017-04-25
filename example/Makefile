
READ_FASTA_BIN=../scripts/read_fasta.py
MAX_COMSUB_SEQ_BIN=../build/bin/max_comsubseq

.PHONY: test
test:
	-[ ! -f influenza.faa ] || [ ! -f protein.fa ] && tar xf input.tar.bz2 && mv input/* ./
	${READ_FASTA_BIN}  influenza.faa
	${READ_FASTA_BIN}  protein.fa
	${MAX_COMSUB_SEQ_BIN} influenza_seq_css.txt protein_seq_css.txt result.bin

.PHONY: small_test
small_test:
	cd small && make

.PHONY: clean
clean:
	-rm -rf input
	-rm influenza.faa
	-rm protein.fa
	-rm -f *.txt
	-rm -f subhash*



