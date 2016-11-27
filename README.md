# PCPE2 - Parallel Common Peptide Explorer v2.0

![travis-ci](https://travis-ci.org/yen3/pcpe2.svg?branch=master)

* The original idea is from Assocaite Professor [Guang-Wu Chen](http://rcevi.cgu.edu.tw/files/14-1065-4045,r639-1.php).
* The program is working for a stable version currently.

## How to run the project

1. Clone the project

```sh
git clone  --recursive https://github.com/yen3/pcpe2 pcpe2
```

2. Build the project
```
make build
```

3. Test the project
```
make unit_test
```

4. Run an example
```
# Take a long while.
make example
```

5. Release the project
```
make release
```

## Requirement

* The compiler need support C++11
    * gcc >= 4.8
    * clang >= 3.4
