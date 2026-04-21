# 不变多项式分析软件

## 开源许可

本软件采用 MIT 许可证开源。

```
MIT License

Copyright (c) 2025 郝一平 娄敬

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

本软件使用C++编写。Linux/Unix环境使用g++编译，Windows环境使用MSVC工具链编译。

### 环境要求

- **Linux/Unix**：g++ (支持C++03标准)
- **Windows**：MSVC（cl.exe、link.exe、nmake）
- **线程支持**：pthread库（并行后端启用时需要）

### Windows 编译（MSVC）

在Windows下请使用 Visual Studio Developer Command Prompt 执行构建命令。

推荐步骤：

```cmd
cd /d d:\projs\FIanalyzer
nmake /nologo /f tools\Makefile.msvc
test\main.exe parameter\input.txt .\output\
```

清理编译产物：

```cmd
nmake /nologo /f tools\Makefile.msvc clean
```

也可直接运行一键脚本：

```cmd
build_run_test.bat
```

注意：如果提示找不到 `nmake` 或 `cl.exe`，说明当前不是 Developer Command Prompt 环境。

### 编译步骤

1. **克隆或下载源代码**
   目前在gitee和github上有开源连接
   ```bash
   cd /path/to/your/workspace
   git clone https://gitee.com/hao-yiping/FIanalyzer.git
   cd FIanalyzer
   ```

   ```bash
   cd /path/to/your/workspace
   git clone https://github.com/YipingHao/PIPFIanalyzer.git
   cd PIPFIanalyzer
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
- **Windows工具缺失**：请确认在 Developer Command Prompt 中运行，且 `nmake`、`cl.exe` 可用

## 使用说明

本软件支持以下运行模式：`dataswitch`、`CodeGeneration`、`cutoff`、`SFI`、`test`。

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
  PIPFileName = "./data/CH4.txt";          // PIP表达式文件路径
  DataFileName = "./data/vain.txt";        // 数据文件路径（仅dataswitch需要）
  OutputFileName = "L-CH4";                // 输出文件前缀（会自动添加.txt/.c/.f90后缀）
  threadCount = 32;                         // 线程参数（仅dataswitch入口使用）

  item = "cutoff";                         // 运行模式：test/dataswitch/CodeGeneration/cutoff/SFI

  CodeGenSetting =
  {
    CcodePrint = true;
    FortranCodePrint = true;
  };

  CutoffSetting =
  {
    threshold = "workload";              // order 或 workload
    order = 5;
    workload = 1000;
    CrossItem = true;
  };

  SFI =
  {
    order = 2;
    poly = 12;
    reciprocal = 12;
    exp = 12;
    gaussian = 12;
    cos = 12;
    sin = 12;
  };

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
- 分组参数使用点路径访问，例如`CodeGenSetting.CcodePrint`、`CutoffSetting.threshold`。



### 运行模式

#### 数据转换模式（item = "dataswitch"）

这是软件的主要功能，用于将键长坐标-能量数据点转换为PIP多项式数据。

```bash
./run.sh -i ./parameter/input.txt -o ./output/
```
**如何启用本模式**：
- 在参数文件中设置`item = "dataswitch"`

**输入文件**：
- `PIPFileName`：PIP表达式文件路径
- `DataFileName`：键长坐标-能量数据点文件

**输入数据格式要求**：
- 每行包含相同数量的数值（浮点数或整数）
- 数值之间用空格分隔
- 支持空行（会被自动忽略）
- 支持注释（以`//`或`/* */`开始的行）
- 数据格式有两种：
  1. 仅包含键长坐标：每行有`XCount`个数值
  2. 包含键长坐标和能量值：每行有`XCount + 1`个数值，最后一个数值为能量值
  3. 其他列数会报错并退出
- 其中`XCount`由PIP表达式中出现的最大变量下标加1自动推导得到

**输出文件**：
- `<OutputFileName>.txt`：转换后的数据文件

**输出数据格式**：
- 每行包含`N + (E)`个数值，其中`N`是PIP多项式的数量，`E`是能量值（如果输入包含能量）
- 如果输入包含能量列，输出最后一列保留原始能量值
- 数值使用科学计数法格式：`%25.16E`
- 返回码约定：`0`成功，`-1`输入输出行数不匹配，`-3`输入列数与`XCount`不一致，`-4`输出列数与多项式数量不一致

**注意**：
- 多线程接口仅在数据转换模式入口使用
- 代码生成功能请使用下方 `CodeGeneration` 独立模式

#### 代码生成模式（item = "CodeGeneration"）

此模式用于将PIP表达式直接转换为C或Fortran代码，不进行数据矩阵转换。

**如何启用本模式**：
- 在参数文件中设置`item = "CodeGeneration"`

**输入文件**：
- `PIPFileName`：PIP表达式文件路径

**输出文件**：
- `<OutputFileName>.c`：生成的C语言代码（如果`CodeGenSetting.CcodePrint = true`）
- `<OutputFileName>.f90`：生成的Fortran语言代码（如果`CodeGenSetting.FortranCodePrint = true`）

**说明**：
- 此模式不依赖`DataFileName`
- `threadCount`对该模式无实际影响

#### 多项式截断模式（item = "cutoff"）

此模式用于对输入的PIP表达式集合做筛选截断，生成较小规模的表达式文件。

**如何启用本模式**：
- 在参数文件中设置`item = "cutoff"`

**输入文件**：
- `PIPFileName`：PIP表达式文件路径

**截断参数（CutoffSetting）**：
- `threshold`：可选`"order"`或`"workload"`
- `order`：当`threshold`为`"order"`时生效，表示最大保留次数
- `workload`：当`threshold`为`"workload"`时生效，表示最大保留工作量
- 当前实现中，`workload`按多项式复杂度累加估算，单项复杂度为`order × itemCount`
- `CrossItem`：是否启用交叉项过滤
- 当前判定逻辑会检查代表单项式中是否存在重复变量索引，若有重复则该项被视为非`CrossItem`

**输出文件**：
- `<OutputFileName>.txt`：截断后的多项式表达式文本（保持`P[i] = ...`原格式）

**说明**：
- 该模式不进行数据矩阵转换
- 该模式当前不执行代码生成
- `cutoff`会触发次数分布与划分信息重建，输出表达式顺序可能与输入顺序不同

#### 基本不变量模式（item = "SFI"）

该模式当前为预留入口，尚未实现具体功能。
参数文件中的`SFI`配置段会被读取，但当前版本不会产出实质计算结果。



#### 测试模式（item = "test"）

此模式仅给软件开发人员使用，用于测试功能：

```bash
./run.sh -i ./parameter/input.txt -o ./output/
```

该模式用于内部测试入口，普通用户一般无需使用。

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

`threadCount`参数在数据转换模式入口中可配置。
当前版本为保证结果一致性，带线程参数的计算接口回退到单线程计算路径。
因此该参数目前主要用于接口兼容和后续并行后端扩展。

```json
threadCount = 32;  // 当前版本下会走确定性单线程计算路径
```

### 使用示例

**数据转换**

1. 准备PIP表达式文件`my_pip.txt`和键长坐标-能量数据点文件
2. 创建参数文件`my_input.txt`：
   ```json
   {
       PIPFileName = "./data/my_pip.txt";
       DataFileName = "./data/O4.txt";
       OutputFileName = "my_output";
       threadCount = 16;  // 使用16个线程进行数据转换
       item = "dataswitch";  // 数据转换模式
   }
   ```
3. 运行程序：
   ```bash
   ./run.sh -I my_input.txt -o ./my_output/
   ```
4. 输出文件：
   - `./my_output/my_output.txt`：转换后的数据文件

**代码生成（独立模式）**

1. 准备PIP表达式文件`my_pip.txt`
2. 创建参数文件`my_codegen.txt`：
  ```json
  {
     PIPFileName = "./data/my_pip.txt";
     OutputFileName = "my_codegen";
     item = "CodeGeneration";
     CodeGenSetting = {
        CcodePrint = true;
        FortranCodePrint = true;
     };
  }
  ```
3. 运行程序：
  ```bash
  ./run.sh -I my_codegen.txt -o ./my_output/
  ```
4. 输出文件：
  - `./my_output/my_codegen.c`：生成的C语言代码
  - `./my_output/my_codegen.f90`：生成的Fortran语言代码

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

- **代码生成模式**：
  - `<OutputFileName>.c`：生成的C语言代码（如果`CodeGenSetting.CcodePrint = true`）
  - `<OutputFileName>.f90`：生成的Fortran语言代码（如果`CodeGenSetting.FortranCodePrint = true`）

- **多项式截断模式**：
  - `<OutputFileName>.txt`：截断后的PIP表达式文本

- **测试模式**：
  - 测试输出文件（仅开发人员使用）

### 注意事项

1. 参数文件中的键名（如`PIPFileName`、`DataFileName`等）必须保持不变
2. 字符串值需要用双引号括起来
3. 确保输入文件路径正确
4. 输出目录如果不存在会自动创建
5. 当前版本中`threadCount`主要用于接口兼容和后续并行后端扩展，不直接代表实际并行线程数

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
  - 提供`cutoffByOrder()`和`cutoffByWorkload()`方法用于特征截断
  - 提供`print()`方法导出原始PIP文本格式
  - 提供`CrossItem`分析与一阶划分信息重建能力

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
- 保留多线程计算接口与任务拆分逻辑
- 在默认构建路径下，线程参数接口用于兼容入口
- 当前计算主路径以确定性结果为优先，可回退到单线程计算

以下策略为并行后端启用时的分配逻辑说明。

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
转换后的数据文件（.txt）           C/Fortran代码文件（.c/.f90）
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


