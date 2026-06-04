# Translator-interpretator

## Overview

This project implements a translator-interpreter for a custom (Pascal-like) programming language. It consists of a lexical analyzer (finite automaton), a syntax analyzer (stack automaton + table-driven RPN generator), and an RPN interpreter

## Documentation

Artifacts/ folder includes the task for creating translator-interpretator and all the artifacts that were used to build it. Link to the Google Drive folder with artifacts: [Artifacts in Google Drive](https://drive.google.com/drive/folders/18g7dTZFj2QGKKR__A-MO6BJPWPQL112d?usp=sharing)

## How to run

1) Compile the translator-interpreter using C++ compiler:

    ```bash
    g++ main.cpp lexer/lexer.cpp parser/parser.cpp interpreter/interpreter.cpp -o compiler
    ```

2) Run your code (you can place it in the default location tests/code.txt). Use '--verbose' flag to see how lexer, parser and interpreter work:

    ```bash
    compiler tests/code.txt --verbose
    ```
