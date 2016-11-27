.PHONY: build
build:
	cd ./src/main_compare; make -j4

.PHONY: unit_test
unit_test:
	make build_unit_test
	cd ./src/main_compare/test; make test

.PHONY: build_unit_test
build_unit_test:
	cd ./src/main_compare/test; ./init_test.sh; make -j4;

.PHONY: release
release:
	cd ./src/main_compare; make clean; make release

.PHONY: example
example:
	cd ./example; make test

.PHONY: clean
clean:
	cd ./src/main_compare; make clean
	cd ./src/main_compare/test; make clean
	cd ./example; make clean

