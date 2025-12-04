ECE467 Lab 5 Submission

Compiler implementation for MiniGLSL to SPIR-V.

Features:
- Full support for MiniGLSL subset
- Correct handling of vector constructors (argument order fix)
- Proper predication using OpSelect for all conditionals
- Support for nested if/else and scopes
- Correct implementation of built-in functions (dp3, lit, rsq)

Directory Structure:
- ./*.c, ./*.h, ./*.l, ./*.y : Source code
- Makefile : Build script
- output/ : Contains the generated .spvasm and .spv files for the 5 reference shaders

To build:
    make

To run:
    ./compiler467 < input.frag > output.spvasm

Generated Files (in output/):
- flat.spv / flat.spvasm
- lambert.spv / lambert.spvasm
- phong.spv / phong.spvasm
- rings.spv / rings.spvasm
- grid.spv / grid.spvasm
