# Pattern-based File Processor

This is a C++ program that reads a file, processes its lines based on specified patterns and actions, and outputs the results. The program supports a variety of operations, including comparisons and logical operations on the parsed tokens.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Usage](#usage)
- [Pattern Syntax](#pattern-syntax)
- [Action Syntax](#action-syntax)
- [Examples](#examples)
- [Building the Project](#building-the-project)

## Introduction

The Pattern-based File Processor reads lines from a file, applies user-defined patterns to each line, and performs specified actions if the patterns match. The patterns can include logical conditions and comparisons. Actions define how the matched lines are processed and output.

## Features

- **Pattern Matching**: Supports complex patterns using logical operators.
- **Comparison Operators**: Supports `<`, `>`, `<=`, `>=`, `==`, and `!=` for comparing fields.
- **Logical Operators**: Supports `AND` and `OR` logical operations between multiple patterns.
- **Field Access**: Supports accessing specific fields within a line using `$` notation.
- **Print Statements**: Allows printing specific fields or entire lines.

## Usage

```bash
./pattern_processor "<pattern { action }>" <filename>
```

### Arguments

- `<pattern { action }>`: A string that specifies the pattern and action to be applied.
- `<filename>`: The name of the file to process.

## Pattern Syntax

Patterns specify the conditions that each line must meet for the actions to be executed. Patterns can include logical and comparison operators.

- **Logical Operators**: 
  - `&`: Logical AND (default)
  - `|`: Logical OR
- **Comparison Operators**:
  - `>`: Greater than
  - `<`: Less than
  - `>=`: Greater than or equal to
  - `<=`: Less than or equal to
  - `==`: Equal to
  - `!=`: Not equal to
- **Special Patterns**:
  - `1`: Always true
  - `0`: Always false
  - `/regex/`: Checks if any token contains the substring `regex` (only supports /<substr>/ and /./)
  - `NR>x`: Line number is greater than `x`
  - `NR<x`: Line number is less than `x`
  - `NR=x`: Line number is equal to `x`
- **Field Access**: 
  - `$n`: Accesses the nth field (1-based index)

Note that the logical operations are very basic, and do not support complex chaining.

## Action Syntax

Actions define what to do with lines that match the patterns. Currently, only `print` statements are supported.

- **Print Statement**: 
  - `print $n`: Prints the nth field.
  - `print $0`: Prints the entire line.

## Examples

### Example 1: Print lines where the first field is greater than 5
```bash
./pattern_processor "$1>5 { print $0 }" example.txt
```

### Example 2: Print lines where the second field is "hello" or the line number is greater than 10
```bash
./pattern_processor "$2==hello|NR>10 { print $0 }" example.txt
```

## Building the Project

To build the project, use the following commands:

```bash
g++ -o pattern_processor main.cpp
```

This will compile the source code and produce an executable named `pattern_processor`.

---
