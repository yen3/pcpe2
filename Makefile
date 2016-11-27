build:
	cd ./src/main_compare; make -j4

unit_test:
	cd ./src/main_compare/test; ./init_test.sh; make -j4; make test

release:
	cd ./src/main_compare; make release

example:
	cd ./example; make test
