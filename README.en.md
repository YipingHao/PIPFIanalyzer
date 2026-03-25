# Invariant Polynomial Analysis Software

## Open Source License

This software is open source under the MIT License.

```
MIT License

Copyright (c) 2025 Hao Yiping

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Introduction
A tool for computing invariant polynomial potential energy surface fitting in computational chemistry. Invariants refer to Permutation Invariant Polynomials (PIP) and Fundamental Invariants (FI).

### Introduction to Invariant Polynomials (PIP) and Fundamental Invariants (FI)

#### Invariant Polynomials (PIP)

Permutation Invariant Polynomials (PIP) are a class of polynomials that remain invariant under molecular permutation transformations. In computational chemistry, PIP is mainly used to describe the relationship between molecular geometry and energy, especially playing an important role in potential energy surface fitting.

- **Core Features**: For any permutation of atoms in a molecule, the value of PIP remains unchanged, which allows it to capture the intrinsic structural features of the molecule without being affected by atom numbering.
- **Application Scenarios**: Mainly used in computational chemistry fields such as constructing molecular potential energy surfaces, reaction path analysis, and conformational search.
- **Advantages**: Compared to traditional coordinate representations, PIP provides a more compact and physically meaningful way to describe molecules, helping to improve computational efficiency and fitting accuracy.

#### Fundamental Invariants (FI)

Fundamental Invariants (FI) are algebraic generators that constitute invariant polynomials. They are a set of algebraically independent invariants that can generate all possible invariant polynomials through algebraic combinations (finite arbitrary addition and multiplication).

- **Definition**: FI is a minimal set of invariants such that any PIP can be expressed as a polynomial function of these FIs.
- **Calculation Method**: Usually calculated through group theory methods, considering the symmetry of molecules and irreducible representations of permutation groups.
- **Importance**: Choosing an appropriate FI set is crucial for building efficient and accurate potential energy surfaces, directly affecting computational efficiency and fitting quality.

#### Relationship between PIP and FI

PIP and FI are closely related concepts:

- FI is the foundation of PIP, and all PIP can be composed of FI
- Selecting an appropriate FI set can significantly reduce the number of terms to be considered, improving computational efficiency
- Together, they form a mathematical framework for describing the structure-energy relationship of molecules

#### Application in This Software

For existing PIP text, this software can convert it into corresponding polynomial evaluation code, supporting C/fortran languages. It can also convert a large amount of energy and configuration data represented as Cartesian coordinates into PIP polynomial input data.

## Installation Tutorial

This software is written in C++ and requires a g++ compiler for compilation and installation.

### Environment Requirements

- **Compiler**: g++ (supports C++03 standard)
- **Operating System**: Linux/Unix system
- **Thread Support**: pthread library (needed for multi-threaded computing)

### Compilation Steps

1. **Clone or download the source code**
   Currently, there is only an open source link on gitee
   ```bash
   cd /path/to/your/workspace
   git clone https://gitee.com/hao-yiping/FIanalyzer.git
   cd FIanalyzer
   ```

2. **Run the installation script**
   ```bash
   ./install.sh
   ```

   By default, the installation script will compile with `-O2` optimization level.

3. **Optional compilation options**

   - **Specify optimization level** (0, 1, 2, 3):
     ```bash
     ./install.sh -o 3
     ```

   - **Enable debug information**:
     ```bash
     ./install.sh -g
     ```

   - **Combine optimization and debugging**:
     ```bash
     ./install.sh -g -o 1
     ```

   - **Clean compilation files and recompile**:
     ```bash
     ./install.sh -c
     ```

   - **Only clean compilation files**:
     ```bash
     ./install.sh -C
     ```

4. **Verify installation**

   After successful compilation, a `test.exe` executable file will be generated in the `test/` directory, and a `libFIanalyzer.a` static library file will be generated in the project root directory.

### Manual Compilation (Optional)

If manual compilation is needed, you can use the makefile in the tools directory:

```bash
cd tools
make
make test.exe
```

### Common Issues

- **Compilation errors**: Ensure that the g++ version supports the C++03 standard
- **Linking errors**: Check if the pthread library is installed (usually installed by default on Linux systems)

## Usage Instructions

This software provides two main functions: converting PIP text into executable code, and converting Cartesian coordinate data into PIP polynomial data.

### Basic Running Method

Use the `run.sh` script to run the program:

```bash
./run.sh [options]
```

### Command Line Options

- `-i <input_file>`: Specify input parameter file (default: `./parameter/input.txt`)
- `-I <filename>`: Specify input parameter file (path is `./parameter/<filename>`)
- `-o <output_file>`: Specify output directory (default: `./output/`)
- `-g [gdb_options...]`: Run with gdb debugging
- `-h`: Display help information

### Parameter File Configuration

The parameter file uses a JSON-like format, with the main configuration items as follows:

```json
{
    PIPFileName = "./data/O4.txt";      // PIP expression file path
    DataFileName = "./data/vain.txt";      // Bond length coordinate-energy data point file
    OutputFileName = "L";                 // Output file prefix (will automatically add .txt/.c/.f suffix)
    threadCount = 32;                     // Number of threads (only used in data conversion mode)
    item = "dataswitch";                  // Running mode: "dataswitch" (main function) or "test" (development test)
    CcodePrint = true;                    // Whether to generate C code (additional function)
    FortranCodePrint = true;              // Whether to generate Fortran code (additional function)
    TestItem = {
        item = 3;                         // Test item number (only used in test mode)
    };
}
```

#### C-style Comments

The parameter file supports C-style comments, including single-line comments (`//`) and multi-line comments (`/* */`). For example:

```json
{
    PIPFileName = "./data/vain.txt";      // This is a single-line comment
    /* This is
       multi-line
       comment */
    DataFileName = "./data/O4.txt";
}
```

You can temporarily disable a parameter item by commenting it out, for example:

```json
{
   CcodePrint = true; 
   //CcodePrint = false; 
}
```
During runtime, the setting `CcodePrint = false` is commented out and will not generate C language code files.

#### Format Requirements

- All parameter items must end with a semicolon (`;`).
- Parameter item keys must be unique and cannot be repeated.
- Parameter item values can be strings, integers, floating-point numbers, or boolean values.
- Boolean values must be `true` or `false`, not other strings.

### Running Modes

#### Data Conversion Mode (item = "dataswitch")

This is the main function of the software, used to convert bond length coordinate-energy data points into PIP polynomial data. It can also convert PIP expressions into C language code or Fortran language code as a side function:

```bash
./run.sh -i ./parameter/input.txt -o ./output/
```
**How to enable this mode**:
- Set `item = "dataswitch"` in the parameter file

**Input files**:
- `PIPFileName`: PIP expression file path
- `DataFileName`: Bond length coordinate-energy data point file

**Input data format requirements**:
- Each line contains the same number of values (floating-point or integer)
- Values are separated by spaces
- Empty lines are supported (will be automatically ignored)
- Comments are supported (lines starting with `//` or `/* */`)
- There are two data formats:
  1. Only contains bond length coordinates: each line has `XCount` values
  2. Contains bond length coordinates and energy values: each line has `XCount + 1` values, with the last value being the energy value

**Output files**:
- `<OutputFileName>.txt`: Converted data file
- `<OutputFileName>.c`: Generated C language code (if `CcodePrint = true`)
- `<OutputFileName>.f`: Generated Fortran language code (if `FortranCodePrint = true`)

**Output data format**:
- Each line contains `N + (E)` values, where `N` is the number of PIP polynomials, and `E` is the energy value (if input includes energy)
- Values use scientific notation format: `%25.16E`

**How to enable code generation**:
- Set `CcodePrint = true` in the parameter file to enable C code generation
- Set `FortranCodePrint = true` in the parameter file to enable Fortran code generation
- Both options can be enabled simultaneously

**Note**:
- Multi-threading functionality is only used in data conversion mode
- Code generation is an additional function of data conversion

#### Test Mode (item = "test")

This mode is only for software developers to test functions:

```bash
./run.sh -i ./parameter/input.txt -o ./output/
```

Ordinary users do not need to use this mode.

### PIP Expression File Format

The PIP expression file contains multiple polynomial definitions, formatted as follows:

```
P[0] = r[0] + r[1] + r[2] + r[3] + r[4] + r[5];
P[1] = r[0] * r[1] + r[0] * r[2] + r[0] * r[3] + ...;
P[2] = r[0] * r[5] + r[1] * r[4] + r[2] * r[3];
...
```

Where:
- `P[i]`: The i-th PIP polynomial
- `r[j]`: The j-th variable (usually bond length or other geometric parameters)
- Parentheses can be used instead of square brackets to denote subscripts
- p and r can be replaced with any C language-supported variable names (identifiers)
- Supported operators: `+` (addition), `*` (multiplication)

### Multi-threaded Computing

The software supports multi-threaded computing in data conversion mode to improve performance. Set the `threadCount` parameter in the parameter file:

```json
threadCount = 32;  // Use 32 threads (only used in data conversion mode)
```

### Usage Examples

**Data Conversion (with code generation)**

1. Prepare PIP expression file `my_pip.txt` and bond length coordinate-energy data point file
2. Create parameter file `my_input.txt`:
   ```json
   {
       PIPFileName = "./data/my_pip.txt";
       DataFileName = "./data/O4.txt";
       OutputFileName = "my_output";
       threadCount = 16;  // Use 16 threads for data conversion
       item = "dataswitch";  // Use main function mode
       CcodePrint = true;  // Also generate C code
       FortranCodePrint = false;  // Do not generate Fortran code
   }
   ```
3. Run the program:
   ```bash
   ./run.sh -I my_input.txt -o ./my_output/
   ```
4. Output files:
   - `./my_output/my_output.txt`: Converted data file
   - `./my_output/my_output.c`: Generated C language code

### Debug Mode

Use gdb to debug the program:

```bash
./run.sh -g
```

You can also specify input and output files here:

```bash
./run.sh -g -i input_file -o  output_file
```

### Output File Description

- **Data conversion mode**:
  - `<OutputFileName>.txt`: Converted data file
  - `<OutputFileName>.c`: Generated C language code (if `CcodePrint = true`)
  - `<OutputFileName>.f`: Generated Fortran language code (if `FortranCodePrint = true`)

- **Test mode**:
  - Test output files (only for developers)

### Notes

1. The key names in the parameter file (such as `PIPFileName`, `DataFileName`, etc.) must remain unchanged
2. String values need to be enclosed in double quotes
3. Ensure the input file path is correct
4. The output directory will be automatically created if it does not exist
5. When using multi-threaded computing, the number of threads should be set reasonably based on the number of CPU cores

## Software Architecture

This software adopts a modular design, mainly including the following core modules:

### Overall Architecture Diagram

```
FIanalyzer/
├── code/              # Core code directory
│   ├── analyzer.h/cpp      # Invariant polynomial analyzer
│   ├── parser.cpp          # PIP expression parser
│   ├── printer.cpp         # Code generator (C/Fortran)
│   ├── analyzerthread.cpp  # Multi-threaded computing module
│   ├── sheet.h/cpp         # Lexical and syntactic analysis table
│   ├── automata.h/cpp      # Automaton framework (DFA/NFA)
│   ├── dictionary.h/cpp    # Parameter file parser
│   ├── template.h          # Template definitions
│   └── xyz.cpp             # Coordinate processing module
├── tools/             # Build tools
│   └── makefile            # Compilation configuration file
├── test/              # Test code
│   ├── test.cpp            # Test entry
│   └── main.cpp            # Main program
├── data/              # Data file directory
│   ├── FIlex.txt           # PIP expression lexical/syntactic rules
│   └── *.txt               # PIP expressions and data files
├── parameter/         # Parameter file directory
│   └── input.txt           # Input parameter configuration
├── output/            # Output directory
├── install.sh         # Installation script
└── run.sh             # Run script
```

### Core Module Description

#### 1. Analyzer Module (analyzer.h/cpp)

This is the core module of the software, responsible for the representation and calculation of invariant polynomials.

**Main Classes**:

- **FIexpress**: Represents a single homogeneous polynomial
  - Stores the degree of the polynomial (order)
  - Stores the number of monomial terms (ItemCount)
  - Stores the variable indices of monomials (items)
  - Provides `compute_bare()` method to calculate polynomial values

- **FIexpresses**: Represents a collection of multiple polynomials
  - Manages multiple FIexpress objects
  - Provides `compute()` method for batch data conversion
  - Provides `build()` method to build polynomial expressions from files
  - Provides `printCcode()` and `printFortrancode()` methods to generate code

**Data Structure**:
```
Homogeneous polynomial storage format:
- Does not store constant terms, all terms have coefficients set to 1.0
- items array stores exponent information of monomials
- Size is ItemCount * order
- Each monomial occupies order elements, representing order independent variables
```

#### 2. Parser Module (parser.cpp)

Responsible for parsing PIP expression files and building polynomial objects.

**Main Functions**:
- Uses LR parser to parse PIP expressions
- Parses based on lexical and syntactic analysis tables (sheet.h)
- Converts text expressions into FIexpress objects
- Supports syntax error detection and reporting

**Parsing Process**:
```
PIP text → Lexical analysis → Syntactic analysis → Abstract syntax tree → FIexpress object
```

#### 3. Code Generator Module (printer.cpp)

Responsible for converting polynomial expressions into executable C or Fortran code.

**Main Functions**:
- `printCcode()`: Generates C language code
- `printFortrancode()`: Generates Fortran language code

**Generated Code Features**:
- Each polynomial uses independent temporary variables
- Each monomial is on a separate line to avoid excessively long single lines
- Uses double-precision floating-point numbers for calculations
- C code uses 0-based indexing, Fortran code uses 1-based indexing

#### 4. Multi-threaded Computing Module (analyzerthread.cpp)

Provides multi-threaded data conversion functionality to improve the efficiency of large-scale data processing.

**Main Functions**:
- Supports multi-threaded parallel computing
- Automatically distributes tasks to each thread
- Uses pthread library to implement thread management

**Thread Allocation Strategy**:
```
Total rows: rowi
Number of threads: threadCount
Each thread processes: rowsPerThread = rowi / threadCount
Remaining rows: remainingRows = rowi % threadCount
The first remainingRows threads each process 1 more row
```

#### 5. Lexical and Syntactic Analysis Module (sheet.h/cpp)

Provides lexical and syntactic analysis rules for PIP expressions.

**Main Components**:
- **FIL**: Lexical rule definitions
  - Identifiers
  - Integers
  - Operators (add, multi)
  - Separators (semicolon, colon, brace, etc.)

- **FIG**: Syntactic rule definitions (LR analysis table)
  - ACTION table: Shift-reduce actions
  - GOTO table: State transitions
  - Production rule definitions

**Supported Syntax**:
```
FIS → FI | FIS FI
FI → Lvalue = Rvalue
Lvalue → TOKEN
Rvalue → ITEMS
ITEMS → ITEM | ITEMS + ITEM
ITEM → TOKEN | ITEM * TOKEN
TOKEN → identifier[integer] | identifier(integer)
```

#### 6. Automaton Framework (automata.h/cpp)

Provides a general automaton implementation for lexical and syntactic analysis.

**Main Components**:
- **DFA**: Deterministic Finite Automaton
- **NFA**: Non-deterministic Finite Automaton
- **LR0/LR1**: LR analyzer
- **RegularExp**: Regular expression parsing

**Data Structures**:
- vector: Dynamic array
- tree: Tree structure (for syntax trees)
- list: Linked list
- buffer: Circular buffer

#### 7. Parameter Parsing Module (dictionary.h/cpp)

Responsible for parsing parameter files and extracting configuration information.

**Supported Data Types**:
- Integer (int_)
- Floating-point (float_)
- String (string_)
- Boolean (bool_)
- Dictionary (dictionary_)

**Usage Method**:
```cpp
dictionary dict;
dict.build("parameter/input.txt");
const char* pipFile = dict.search("", "PIPFileName");
int threadCount = dict.search(1, "threadCount");
```

### Data Flow Diagram

```
Input files
┌───────────────────┴───────────────────┐
│                                       │
PIP expression file                  Bond length coordinate-energy data point file
│                                       │
└───────────────────┬───────────────────┘
                    ↓
Parameter file parsing (dictionary)
                    ↓
Lexical analysis (FIL)
                    ↓
Syntactic analysis (FIG/LR)
                    ↓
Build polynomial objects (FIexpresses)
                    ↓
┌───────────────────┴───────────────────┐
│                                       │
Main function: Data conversion (multi-threaded)        Additional function: Code generation
│                                       │
Converted data file (.txt)           C/Fortran code files (.c/.f)
```

### Thread Model

The software adopts a master-slave thread model:

```
Main thread
  ├─ Read parameter file
  ├─ Parse PIP expressions
  ├─ Create worker threads
  └─ Wait for all threads to complete
       │
Worker thread 1 ─┐
Worker thread 2 ─┼─ Parallel computing for data conversion
Worker thread N ─┘
```

### Compilation Products

The following files are generated after compilation:

- **libFIanalyzer.a**: Static library file containing all core functions
- **test/test.exe**: Executable program providing a command-line interface

### Extensibility Design

The software has good extensibility:

1. **Add new code generators**: Refer to printer.cpp to implement new print methods
2. **Support new syntax**: Modify lexical and syntactic rules in sheet.h
3. **Optimize computational performance**: Optimize the compute_bare method in analyzer.cpp
4. **Add new data types**: Add new Ktypes in dictionary.h

### Performance Optimization

- Uses row-major order storage to improve cache hit rate
- Supports multi-threaded parallel computing
- Avoids unnecessary memory allocation and copying
- Uses efficient automaton algorithms for lexical and syntactic analysis

### Error Handling

The software provides a comprehensive error handling mechanism:

- Lexical errors: Report position and error type
- Syntactic errors: Report expected symbols and actual symbols
- Runtime errors: Check input and output dimension matching
- File errors: Check if files exist and are readable
