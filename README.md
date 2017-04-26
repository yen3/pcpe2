# PCPE2 - Parallel Common Peptide Explorer v2.0

![travis-ci](https://travis-ci.org/yen3/pcpe2.svg?branch=master)

* The program can find maximum common subseqences of two protein/ DNA sequences
  files.
* The program is under developement.
* The original design is from Assocaite Professor [Guang-Wu Chen](http://rcevi.cgu.edu.tw/files/14-1065-4045,r639-1.php).

## Recommanded Environment

* RAM: 4 GB or more
* Prerequisite
  * C++ compiler with C++11 support
      * gcc 4.8 or newer
      * clang 3.4 or newer
  * CMake 2.8 or newer

## How to run the project

* Clone the project

```sh
git clone  --recursive https://github.com/yen3/pcpe2 pcpe2
```

* Build the project

```
mkdir build
cd build
cmake ..
make -j4
make check -j4
```

* Run unit tests

```
mkdir build
cd build
cmake ..
make check -j4
```

## License

* BSD-3
