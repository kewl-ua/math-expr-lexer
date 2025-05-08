# Math Expression Lexer

A simple lexer/parser for mathematical expressions, implemented in pure C.

## Project Structure

math-expr-lexer/

├── bin/          # Executable binaries

├── build/        # Temporary build files

├── docs/         # Documentation

├── include/      # Header files (.h)

├── lib/          # External libraries

├── scripts/      # Automation scripts

├── src/          # Source code (.c)

├── tests/        # Unit and integration tests

├── config.h      # Project configuration

├── LICENSE       # Project license

├── README.md     # This file

└── CMakeLists.txt # CMake build configuration

## Requirements
C compiler (gcc, clang, or MSVC)

- C compiler (`gcc`, `clang`, or MSVC)
- [CMake](https://cmake.org/download)
- Make

## Building
Create and enter the build directory:

``` bash
mkdir build
cd build
```

Run CMake to configure the build:

### For Linux/macOS:
``` bash
cmake ..
```

### For Windows (MinGW/MSYS2):
```bash
cmake -G "MSYS Makefiles" ..
```

### Build the project:

``` bash
cmake --build .
```

The executable will appear in the bin directory: bin/math_expr_lexer(.exe)

## Usage

Run the executable from the command line:

``` bash
./bin/math_expr_lexer
```

## Clean

To clean the build artifacts, simply remove the contents of the build and bin directories:

``` bash
rm -rf build/* bin/*
```

