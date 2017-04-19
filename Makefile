.PHONY: build
build:
	cd ./src/max_subseq; make -j4

.PHONY: unit_test
unit_test:
	make build_unit_test
	cd ./src/max_subseq/test; make test

.PHONY: build_unit_test
build_unit_test:
	cd ./src/max_subseq/test; ./init_test.sh; make -j4;

.PHONY: release
release:
	cd ./src/main_compare; make clean; make release

.PHONY: example
example:
	cd ./example; make test

.PHONY: clean
clean:
	cd ./src/max_subseq; make clean
	cd ./src/max_subseq/test; make clean
	cd ./example; make clean

