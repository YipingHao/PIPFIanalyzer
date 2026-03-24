# 不变多项式分析软件

## 开源许可

本软件采用 MIT 许可证开源。

```
MIT License

Copyright (c) 2025 郝一平

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

## 介绍
用于计算化学不变量多项式势能面拟合的工具。不变量是指置换不变多项式(PIP)和基本不变量FI.

### 不变多项式(PIP)和基本不变量FI简介

#### 不变多项式(PIP) 

不变多项式(Perception Invariant Polynomials, PIP)是一类在分子置换变换下保持不变的多项式。在计算化学中，PIP 主要用于描述分子的几何结构和能量之间的关系，特别是在势能面拟合中发挥重要作用。

- **核心特性**：对于分子中原子的任何置换，PIP 的值保持不变，这使得它能够捕捉分子的内在结构特征，而不受原子编号的影响。
- **应用场景**：主要用于构建分子势能面、反应路径分析、构象搜索等计算化学领域。
- **优势**：相比传统的坐标表示，PIP 提供了一种更紧凑、更具物理意义的分子描述方式，有助于提高计算效率和拟合精度。

#### 基本不变量(FI) 

基本不变量(Fundamental Invariants, FI)是构成不变多项式的代数生成元。它们是一组代数无关的不变量，通过代数组合(有限次任意加法和乘法)可以生成所有可能的不变多项式。

- **定义**：FI 是一组最小的不变量集合，使得任何 PIP 都可以表示为这些 FI 的多项式函数。
- **计算方法**：通常通过群论方法计算，考虑分子的对称性和置换群的不可约表示。
- **重要性**：选择合适的 FI 集合对于构建高效、准确的势能面至关重要，直接影响计算效率和拟合质量。

#### PIP与FI的关系

PIP 和 FI 是密切相关的概念：

- FI 是 PIP 的基础，所有 PIP 都可以由 FI 组合而成
- 选择适当的 FI 集合可以显著减少需要考虑的项数，提高计算效率
- 两者共同构成了描述分子结构-能量关系的数学框架

#### 在本软件中的应用

对于已经有的PIP文本，本软件可以将它转换为对应的多项式求值代码，支持C/fortran语言。也可以把大量的表示为笛卡尔坐标的能力和构型数据转换为PIP多项式为输入的数据。





## 安装教程

本软件使用C++编写，需要g++编译器进行编译安装。

### 环境要求

- **编译器**：g++ (支持C++03标准)
- **操作系统**：Linux/Unix系统
- **线程支持**：pthread库（多线程计算需要）

### 编译步骤

1. **克隆或下载源代码**
   目前仅仅在gitee上有开源链接
   ```bash
   cd /path/to/your/workspace
   git clone https://gitee.com/hao-yiping/FIanalyzer.git
   cd FIanalyzer
   ```

2. **运行安装脚本**
   ```bash
   ./install.sh
   ```

   默认情况下，安装脚本会使用`-O2`优化级别进行编译。

3. **可选编译选项**

   - **指定优化级别**（0, 1, 2, 3）：
     ```bash
     ./install.sh -o 3
     ```

   - **启用调试信息**：
     ```bash
     ./install.sh -g
     ```

   - **组合使用优化和调试**：
     ```bash
     ./install.sh -g -o 1
     ```

   - **清理编译文件后重新编译**：
     ```bash
     ./install.sh -c
     ```

   - **仅清理编译文件**：
     ```bash
     ./install.sh -C
     ```

4. **验证安装**

   编译成功后，会在`test/`目录下生成`test.exe`可执行文件，同时在项目根目录下生成`libFIanalyzer.a`静态库文件。

### 手动编译（可选）

如果需要手动编译，可以使用tools目录下的makefile：

```bash
cd tools
make
make test.exe
```

### 常见问题

- **编译错误**：确保g++版本支持C++03标准
- **链接错误**：检查pthread库是否已安装（Linux系统通常默认安装）

## 使用说明

本软件提供两种主要功能：将PIP文本转换为可执行代码，以及将笛卡尔坐标数据转换为PIP多项式数据。

### 基本运行方式

使用`run.sh`脚本运行程序：

```bash
./run.sh [选项]
```

### 命令行选项

- `-i <input_file>`：指定输入参数文件（默认：`./parameter/input.txt`）
- `-I <filename>`：指定输入参数文件（路径为`./parameter/<filename>`）
- `-o <output_file>`：指定输出目录（默认：`./output/`）
- `-g [gdb_options...]`：使用gdb调试运行
- `-h`：显示帮助信息

### 参数文件配置

参数文件采用类似JSON的格式，主要配置项如下：

```json
{
    PIPFileName = "./data/O4.txt";      // PIP表达式文件路径
    DataFileName = "./data/vain.txt";      // 键长坐标-能量数据点文件
    OutputFileName = "L";                 // 输出文件前缀（会自动添加.txt/.c/.f后缀）
    threadCount = 32;                     // 线程数量（仅用于数据转换模式）
    item = "dataswitch";                  // 运行模式："dataswitch"（主要功能）或"test"（开发测试）
    CcodePrint = true;                    // 是否生成C代码（附加功能）
    FortranCodePrint = true;              // 是否生成Fortran代码（附加功能）
    TestItem = {
        item = 3;                         // 测试项编号（仅用于test模式）
    };
}
```

#### C风格注释

参数文件支持C风格的注释，包括单行注释（`//`）和多行注释（`/* */`）。例如：

```json
{
    PIPFileName = "./data/vain.txt";      // 这是单行注释
    /* 这是
       多行
       注释 */
    DataFileName = "./data/O4.txt";
}
```

可以通过注释来暂时屏蔽某个参数项，例如：

```json
{
   CcodePrint = true; 
   //CcodePrint = false; 
}
```
在运行时，CcodePrint = false这个设置被注释，不会生成C语言代码文件。

#### 格式要求

- 所有参数项末尾必须有分号（`;`）。
- 参数项的键名必须是唯一的，不能重复。
- 参数项的值可以是字符串、整数、浮点数、布尔值。
- 布尔值必须是`true`或`false`，不能是其他字符串。



### 运行模式

#### 数据转换模式（item = "dataswitch"）

这是软件的主要功能，用于将键长坐标-能量数据点转换为PIP多项式数据。也可在完成功能时顺便将PIP表达式转换为C语言代码或者Fortran语言代码：

```bash
./run.sh -i ./parameter/input.txt -o ./output/
```
**如何启用本模式**：
- 在参数文件中设置`item = "dataswitch"`

**输入文件**：
- `PIPFileName`：PIP表达式文件路径
- `DataFileName`：键长坐标-能量数据点文件

**输出文件**：
- `<OutputFileName>.txt`：转换后的数据文件
- `<OutputFileName>.c`：生成的C语言代码（如果`CcodePrint = true`）
- `<OutputFileName>.f`：生成的Fortran语言代码（如果`FortranCodePrint = true`）

**如何启用代码生成**：
- 在参数文件中设置`CcodePrint = true`启用C代码生成
- 在参数文件中设置`FortranCodePrint = true`启用Fortran代码生成
- 两个选项可以同时启用

**注意**：
- 多线程功能仅用于数据转换模式
- 代码生成是数据转换的附加功能

#### 测试模式（item = "test"）

此模式仅给软件开发人员使用，用于测试功能：

```bash
./run.sh -i ./parameter/input.txt -o ./output/
```

普通用户无需使用此模式。

### PIP表达式文件格式

PIP表达式文件包含多个多项式定义，格式如下：

```
P[0] = r[0] + r[1] + r[2] + r[3] + r[4] + r[5];
P[1] = r[0] * r[1] + r[0] * r[2] + r[0] * r[3] + ...;
P[2] = r[0] * r[5] + r[1] * r[4] + r[2] * r[3];
...
```

其中：
- `P[i]`：第i个PIP多项式
- `r[j]`：第j个变量（通常是键长或其他几何参数）
- 可以使用圆括号代替方括号来表示下标
- p和r可以换成任意的C语言支持的变量名（标识符）
- 支持的运算符：`+`（加法）、`*`（乘法）

### 多线程计算

软件在数据转换模式下支持多线程计算以提高性能。在参数文件中设置`threadCount`参数：

```json
threadCount = 32;  // 使用32个线程（仅用于数据转换模式）
```

### 使用示例

**数据转换（带代码生成）**

1. 准备PIP表达式文件`my_pip.txt`和键长坐标-能量数据点文件
2. 创建参数文件`my_input.txt`：
   ```json
   {
       PIPFileName = "./data/my_pip.txt";
       DataFileName = "./data/O4.txt";
       OutputFileName = "my_output";
       threadCount = 16;  // 使用16个线程进行数据转换
       item = "dataswitch";  // 使用主要功能模式
       CcodePrint = true;  // 同时生成C代码
       FortranCodePrint = false;  // 不生成Fortran代码
   }
   ```
3. 运行程序：
   ```bash
   ./run.sh -I my_input.txt -o ./my_output/
   ```
4. 输出文件：
   - `./my_output/my_output.txt`：转换后的数据文件
   - `./my_output/my_output.c`：生成的C语言代码

### 调试模式

使用gdb调试程序：

```bash
./run.sh -g
```

也可以在这里指定输入输出文件：

```bash
./run.sh -g -i input_file -o  output_file
```

### 输出文件说明

- **数据转换模式**：
  - `<OutputFileName>.txt`：转换后的数据文件
  - `<OutputFileName>.c`：生成的C语言代码（如果`CcodePrint = true`）
  - `<OutputFileName>.f`：生成的Fortran语言代码（如果`FortranCodePrint = true`）

- **测试模式**：
  - 测试输出文件（仅开发人员使用）

### 注意事项

1. 参数文件中的键名（如`PIPFileName`、`DataFileName`等）必须保持不变
2. 字符串值需要用双引号括起来
3. 确保输入文件路径正确
4. 输出目录如果不存在会自动创建
5. 多线程计算时，线程数应根据CPU核心数合理设置

## 软件架构

本软件采用模块化设计，主要包含以下几个核心模块：

### 整体架构图

```
FIanalyzer/
├── code/              # 核心代码目录
│   ├── analyzer.h/cpp      # 不变量多项式分析器
│   ├── parser.cpp          # PIP表达式解析器
│   ├── printer.cpp         # 代码生成器（C/Fortran）
│   ├── analyzerthread.cpp  # 多线程计算模块
│   ├── sheet.h/cpp         # 词法和语法分析表
│   ├── automata.h/cpp      # 自动机框架（DFA/NFA）
│   ├── dictionary.h/cpp    # 参数文件解析器
│   ├── template.h          # 模板定义
│   └── xyz.cpp             # 坐标处理模块
├── tools/             # 构建工具
│   └── makefile            # 编译配置文件
├── test/              # 测试代码
│   ├── test.cpp            # 测试入口
│   └── main.cpp            # 主程序
├── data/              # 数据文件目录
│   ├── FIlex.txt           # PIP表达式词法/语法规则
│   └── *.txt               # PIP表达式和数据文件
├── parameter/         # 参数文件目录
│   └── input.txt           # 输入参数配置
├── output/            # 输出目录
├── install.sh         # 安装脚本
└── run.sh             # 运行脚本
```

### 核心模块说明

#### 1. 分析器模块（analyzer.h/cpp）

这是软件的核心模块，负责不变量多项式的表示和计算。

**主要类**：

- **FIexpress**：表示单个齐次多项式
  - 存储多项式的次数（order）
  - 存储单项式项数（ItemCount）
  - 存储单项式的变量索引（items）
  - 提供`compute_bare()`方法计算多项式值

- **FIexpresses**：表示多个多项式的集合
  - 管理多个FIexpress对象
  - 提供`compute()`方法进行批量数据转换
  - 提供`build()`方法从文件构建多项式表达式
  - 提供`printCcode()`和`printFortrancode()`方法生成代码

**数据结构**：
```
齐次多项式存储格式：
- 不存储常数项，所有项的系数都设为1.0
- items数组存储单项式的指数信息
- 大小为 ItemCount * order
- 每个单项式占 order 个元素，代表order个自变元
```

#### 2. 解析器模块（parser.cpp）

负责解析PIP表达式文件，构建多项式对象。

**主要功能**：
- 使用LR解析器解析PIP表达式
- 基于词法和语法分析表（sheet.h）进行解析
- 将文本表达式转换为FIexpress对象
- 支持语法错误检测和报告

**解析流程**：
```
PIP文本 → 词法分析 → 语法分析 → 抽象语法树 → FIexpress对象
```

#### 3. 代码生成器模块（printer.cpp）

负责将多项式表达式转换为可执行的C或Fortran代码。

**主要功能**：
- `printCcode()`：生成C语言代码
- `printFortrancode()`：生成Fortran语言代码

**生成的代码特点**：
- 每个多项式使用独立的临时变量
- 每个单项式单独一行，避免单行过长
- 使用双精度浮点数进行计算
- C代码使用0-based索引，Fortran代码使用1-based索引

#### 4. 多线程计算模块（analyzerthread.cpp）

提供多线程数据转换功能，提高大规模数据处理的效率。

**主要功能**：
- 支持多线程并行计算
- 自动分配任务给各个线程
- 使用pthread库实现线程管理

**线程分配策略**：
```
总行数：rowi
线程数：threadCount
每个线程处理：rowsPerThread = rowi / threadCount
剩余行数：remainingRows = rowi % threadCount
前remainingRows个线程各多处理1行
```

#### 5. 词法和语法分析模块（sheet.h/cpp）

提供PIP表达式的词法和语法分析规则。

**主要组件**：
- **FIL**：词法规则定义
  - 标识符（identifier）
  - 整数（integer）
  - 运算符（add, multi）
  - 分隔符（semicolon, colon, brace等）

- **FIG**：语法规则定义（LR分析表）
  - ACTION表：移进-归约动作
  - GOTO表：状态转移
  - 产生式规则定义

**支持的语法**：
```
FIS → FI | FIS FI
FI → Lvalue = Rvalue
Lvalue → TOKEN
Rvalue → ITEMS
ITEMS → ITEM | ITEMS + ITEM
ITEM → TOKEN | ITEM * TOKEN
TOKEN → identifier[integer] | identifier(integer)
```

#### 6. 自动机框架（automata.h/cpp）

提供通用的自动机实现，用于词法和语法分析。

**主要组件**：
- **DFA**：确定性有限自动机
- **NFA**：非确定性有限自动机
- **LR0/LR1**：LR分析器
- **RegularExp**：正则表达式解析

**数据结构**：
- vector：动态数组
- tree：树结构（用于语法树）
- list：链表
- buffer：环形缓冲区

#### 7. 参数解析模块（dictionary.h/cpp）

负责解析参数文件，提取配置信息。

**支持的数据类型**：
- 整数（int_）
- 浮点数（float_）
- 字符串（string_）
- 布尔值（bool_）
- 字典（dictionary_）

**使用方法**：
```cpp
dictionary dict;
dict.build("parameter/input.txt");
const char* pipFile = dict.search("", "PIPFileName");
int threadCount = dict.search(1, "threadCount");
```

### 数据流图

```
输入文件
┌───────────────────┴───────────────────┐
│                                       │
PIP表达式文件                      键长坐标-能量数据点文件
│                                       │
└───────────────────┬───────────────────┘
                    ↓
参数文件解析（dictionary）
                    ↓
词法分析（FIL）
                    ↓
语法分析（FIG/LR）
                    ↓
构建多项式对象（FIexpresses）
                    ↓
┌───────────────────┴───────────────────┐
│                                       │
主要功能：数据转换（多线程）        附加功能：代码生成
│                                       │
转换后的数据文件（.txt）           C/Fortran代码文件（.c/.f）
```

### 线程模型

软件采用主从线程模型：

```
主线程
  ├─ 读取参数文件
  ├─ 解析PIP表达式
  ├─ 创建工作线程
  └─ 等待所有线程完成
       │
工作线程1 ─┐
工作线程2 ─┼─ 并行计算数据转换
工作线程N ─┘
```

### 编译产物

编译后生成以下文件：

- **libFIanalyzer.a**：静态库文件，包含所有核心功能
- **test/test.exe**：可执行程序，提供命令行接口

### 扩展性设计

软件具有良好的扩展性：

1. **添加新的代码生成器**：参考printer.cpp实现新的print方法
2. **支持新的语法**：修改sheet.h中的词法和语法规则
3. **优化计算性能**：在analyzer.cpp中优化compute_bare方法
4. **添加新的数据类型**：在dictionary.h中添加新的Ktype

### 性能优化

- 使用行主序存储，提高缓存命中率
- 支持多线程并行计算
- 避免不必要的内存分配和拷贝
- 使用高效的自动机算法进行词法语法分析

### 错误处理

软件提供完善的错误处理机制：

- 词法错误：报告位置和错误类型
- 语法错误：报告期望的符号和实际符号
- 运行时错误：检查输入输出维度匹配
- 文件错误：检查文件是否存在和可读


