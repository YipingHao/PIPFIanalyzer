# PIPBIN v1 Binary Polynomial Format

> This document is a copy of the same protocol document in FIgenerator. Changes to the
> PIPBIN protocol must update the documentation and implementation in both repositories.

## Scope

PIPBIN transfers expanded homogeneous invariant polynomials from FIgenerator to
PIPFIanalyzer. It represents the same information as `Unified` text output without
identifiers, whitespace, or operators, so loading does not require lexer tokens or an AST.

Version 1 stores homogeneous polynomials with implicit `+1` monomial coefficients and
zero-based unsigned feature indices. Files use the `.pipbin` extension. Every multi-byte
integer is little-endian; no C++ structure is written directly, so the protocol is
independent of ABI padding and host `size_t` width.

## Fixed 64-byte header

| Offset | Size | Type | Field | v1 value or meaning |
|---:|---:|---|---|---|
| 0 | 8 | bytes | magic | `PIPBIN1\0` |
| 8 | 4 | uint32 | version | `1` |
| 12 | 4 | uint32 | header_size | `64` |
| 16 | 4 | uint32 | endian_marker | `0x01020304` |
| 20 | 4 | uint32 | index_width | `4` |
| 24 | 4 | uint32 | content_kind | `1=FI`, `2=NonFI`, `3=complete PIP` |
| 28 | 4 | uint32 | flags | `7` in v1 |
| 32 | 8 | uint64 | feature_count | number of input features `r[i]` |
| 40 | 8 | uint64 | polynomial_count | number of following records |
| 48 | 8 | uint64 | factor_index_count | total number of stored feature indices |
| 56 | 4 | uint32 | max_order | highest generated order |
| 60 | 4 | uint32 | reserved | must be `0` |

Flag bit 0 means coefficients are implicit `+1`, bit 1 means each polynomial is
homogeneous, and bit 2 means records are sorted by nondecreasing order. Readers must at
least require bits 0 and 1.

## Polynomial records

Exactly `polynomial_count` records follow the header. Each starts with:

| Relative offset | Size | Type | Field |
|---:|---:|---|---|
| 0 | 4 | uint32 | order |
| 4 | 4 | uint32 | reserved, must be `0` |
| 8 | 8 | uint64 | monomial_count |

The record then stores `monomial_count * order` uint32 feature indices. Each consecutive
group of `order` indices is one monomial.

The exact v1 file size is:

```text
64 + 16 * polynomial_count + 4 * factor_index_count
```

For example,

```text
p = r[0] * r[0] * r[2] + r[1] * r[3] * r[4]
```

is `order=3`, `monomial_count=2`, followed by `0, 0, 2, 1, 3, 4`.

PIPFIanalyzer validates the header, required flags, counts, orders, index bounds, exact
`factor_index_count`, and exact EOF. Truncated files, trailing data, and inconsistent
counts are rejected.

## Configuration

FIgenerator:

```text
PrintSetting = {
    PrintSetting = "Binary";
    FIfileNamePrefix = "ethanol_pip";
    NonFIfileNamePrefix = "ethanol_nonfi";
    PIPprintSetting = "WithFI";
};
```

PIPFIanalyzer:

```text
PIPFileName = "ethanol_pip.pipbin";
PIPFileFormat = "Binary";
```

With `PIPFileFormat="Auto"`, a `.pipbin` suffix selects the binary loader; all other
suffixes select the text loader.
