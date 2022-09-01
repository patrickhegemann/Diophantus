# Diophantus

Diophantus is a linear integer equation system solver.

The algorithm in its current form is based on the Omega Test [1], which is *not* considered the fastest algorithm for solving equation systems, but has applications in existential quantifier elimination. A more commonly used method would be the Simplex algorithm. For a detailed treatment of this and related topics, refer to [2].

I used this project to learn more about C++20, GoogleTest, CMake, and more.

## Built with

* C++20
* gmp
* googletest
* boost
* argparse

## Setup

Using docker:
```shell
docker build -t diophantus/diophantus_build:0.1 -f Dockerfile .
docker run -it diophantus/diophantus_build:0.1 bash
```

Without docker, on Ubuntu 20.04:

```shell
# Install required packages
sudo apt install build-essential cmake clang libstdc++-10-dev libgmp-dev libboost-log1.71-dev

# Build this project
mkdir build
cd build
CXX=clang++ cmake ..
make
```

## Usage

Run examples:
```shell
./bin/diophantus_cli examples/ex-5-5
./bin/diophantus_cli examples/ex-10-10
```

## References

[1] Pugh, William. "The Omega test: a fast and practical integer programming algorithm for dependence analysis." Supercomputing'91: Proceedings of the 1991 ACM/IEEE conference on Supercomputing. IEEE, 1991.

[2] Kroening, Daniel, and Ofer Strichman. Decision procedures. Springer-Verlag Berlin Heidelberg, 2016.