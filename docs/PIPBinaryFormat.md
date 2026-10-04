# PIPBIN v1 二进制多项式格式

> 本文档是 FIgenerator 中同名协议文档的副本。修改 PIPBIN 协议时，必须同步更新两个仓库中的文档与实现。

## 1. 用途与边界

`PIPBIN` 用于在 FIgenerator 与 PIPFIanalyzer 之间传递已经展开的齐次不变量多项式。
它保存与 `Unified` 文本格式相同的信息，但不保存标识符、空白和运算符，因此文件更小，读取时也不需要
词法分析或构造 AST。

当前版本只表示以下多项式：

- 每个多项式都是齐次的；
- 每个单项式的系数都是 `+1`；
- 变量由从 0 开始的无符号整数下标表示；
- 多项式按 FIgenerator 的输出顺序保存，文件中不另外保存多项式编号。

文件扩展名为 `.pipbin`。所有多字节整数均使用小端序。文件协议不直接写入 C++ 结构体，因此不依赖
结构体填充、编译器 ABI 或宿主机 `size_t` 宽度。

## 2. 文件头

v1 文件头固定为 64 字节：

| 偏移 | 大小 | 类型 | 字段 | v1 取值或含义 |
|---:|---:|---|---|---|
| 0 | 8 | bytes | magic | `PIPBIN1\0` |
| 8 | 4 | uint32 | version | `1` |
| 12 | 4 | uint32 | header_size | `64` |
| 16 | 4 | uint32 | endian_marker | `0x01020304` |
| 20 | 4 | uint32 | index_width | `4` |
| 24 | 4 | uint32 | content_kind | `1=FI`，`2=NonFI`，`3=完整 PIP` |
| 28 | 4 | uint32 | flags | v1 为 `7`，见下文 |
| 32 | 8 | uint64 | feature_count | 输入特征 `r[i]` 的总数 |
| 40 | 8 | uint64 | polynomial_count | 后续多项式记录数 |
| 48 | 8 | uint64 | factor_index_count | 全文件变量下标总数 |
| 56 | 4 | uint32 | max_order | 生成到的最高阶数 |
| 60 | 4 | uint32 | reserved | 必须为 `0` |

`flags` 的低三位含义：

- bit 0：所有单项式系数隐含为 `+1`；
- bit 1：每个多项式是齐次多项式；
- bit 2：记录按阶数非递减排列。

读取器必须至少检查 bit 0 和 bit 1；未知版本或不支持的字段组合应当报错，不能猜测解释。

## 3. 多项式记录

文件头之后紧接 `polynomial_count` 条记录。每条记录先写 16 字节记录头：

| 相对偏移 | 大小 | 类型 | 字段 |
|---:|---:|---|---|
| 0 | 4 | uint32 | order |
| 4 | 4 | uint32 | reserved，必须为 `0` |
| 8 | 8 | uint64 | monomial_count |

随后写入 `monomial_count * order` 个 uint32 下标。每连续 `order` 个下标组成一个单项式。

因此 v1 文件的精确字节数为：

```text
64 + 16 * polynomial_count + 4 * factor_index_count
```

例如三阶多项式

```text
p = r[0] * r[0] * r[2] + r[1] * r[3] * r[4]
```

保存为 `order=3`、`monomial_count=2`，下标序列为：

```text
0, 0, 2, 1, 3, 4
```

## 4. 完整性检查

PIPFIanalyzer 的 v1 读取器会检查：

- magic、版本、文件头大小、字节序标记、下标宽度和保留字段；
- 内容类型和必要 flags；
- 每条记录的阶数及下标是否小于 `feature_count`；
- 实际下标总数是否等于 `factor_index_count`；
- 实际记录数是否等于 `polynomial_count`；
- 最后一条记录后是否恰好到达 EOF。

因此被截断、拼接了多余字节或计数字段不一致的文件会被拒绝。

## 5. FIgenerator 配置

```text
PrintSetting = {
    PrintSetting = "Binary";
    FIfileNamePrefix = "ethanol_pip";
    NonFIfileNamePrefix = "ethanol_nonfi";
    PIPprintSetting = "WithFI";
};
```

程序会生成 `ethanol_pip.pipbin`。`PIPprintSetting` 的语义与文本输出相同：

- `None`：FI 文件只含 FI；
- `Alone`：FI 与 NonFI 分别写入两个 `.pipbin` 文件；
- `WithFI`：FI 文件包含 FI 与 NonFI，即完整 PIP。

## 6. PIPFIanalyzer 配置

```text
PIPFileName = "ethanol_pip.pipbin";
PIPFileFormat = "Binary";
```

`PIPFileFormat="Auto"` 时按 `.pipbin` 扩展名选择二进制读取器，否则选择文本读取器。显式指定
`Text` 或 `Binary` 可以避免扩展名不标准时的歧义。
