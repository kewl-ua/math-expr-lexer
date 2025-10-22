# Math Expression Lexer

A simple lexer for mathematical expressions, implemented in ISO C11. The project is organised
following common C project practices: public headers live under `include/`, reusable code is built
as a static library, and the command-line demo is compiled as a thin frontend on top of the
library.

## Project Structure

```
math-expr-lexer/
├── CMakeLists.txt       # Build configuration
├── include/
│   └── math_expr/       # Public headers
├── src/
│   ├── app/             # Executable entry points
│   └── lexer/           # Library implementation files
├── config.h             # Project configuration stub
├── LICENSE              # Project license
└── README.md            # This document
```

Additional scaffolding folders such as `bin/`, `build/`, `docs/`, `scripts/`, `lib/`, and `tests/`
can be introduced as the project grows. They are not created by default to keep the repository
clean when unused.

## Requirements

- ISO C compiler (`gcc`, `clang`, or MSVC`)
- [CMake](https://cmake.org/download)
- Make or another build tool supported by CMake

## Building

Create and enter the build directory:

```bash
mkdir -p build
cd build
```

Configure the project:

```bash
cmake ..
```

Build the targets:

```bash
cmake --build .
```

The executable will be generated at `bin/math_expr_lexer` (or with `.exe` on Windows).

## Usage

From the project root after building:

```bash
./bin/math_expr_lexer
```

Example output:
```
Expression: 3.1415 * radius^2 + sin(theta / 2)

Tokens:
NUMBER       -> 3.1415     (value = 3.1415)
SPACE        -> [space]
OPERATOR     -> *
...
```

## Library usage

Client code should include `math_expr/lexer.h`, initialise a `math_expr_token_array`, and pass it
to `math_expr_lex_expression`. Remember to clean up with `math_expr_token_array_deinit` when the
token array is no longer needed.

```c
#include "math_expr/lexer.h"

int main(void) {
    math_expr_token_array tokens;
    math_expr_token_array_init(&tokens);

    if (math_expr_lex_expression("2 * (x + 1)", &tokens) != 0) {
        return 1;
    }

    /* consume tokens->data here */

    math_expr_token_array_deinit(&tokens);
    return 0;
}
```

## Cleaning up

To remove build artefacts, delete the `build/` and `bin/` directories:

```bash
rm -rf build bin
```
