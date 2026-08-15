# `symbols.csv`

This file contains all symbols for a module, one per line.

Example file:

```
Address,Size,Type,Symbol
0x00413911,0x1C,func,xnew
0x00418034,0x4,data,?RedirStderr@@3HA
0x004010BC,0x0,imp,__imp__SetConsoleCtrlHandler@8
```

## Format

Numbers can be written as decimal or hexadecimal. Hexadecimal numbers must be prefixed with `0x`.

Comment lines starting with `//` or `#` are permitted, but are currently **not** preserved when updating the file.

```text
Address,Size,Type,Symbol
```

- `Address` - Absolute symbol address, written as decimal or hexadecimal.
- `Size` - Symbol size in bytes, written as decimal or hexadecimal.
- `Type` - One of `func`, `data`, or `imp`.
- `Symbol` - The symbol name. C++ names and exact COFF import names are allowed.

There are no comment attributes. Calling conventions, scope, data hints, and
alignment are not represented in this format.

## x86 MSVC Symbols

For the x86 host compiler, keep C names undecorated in `symbols.txt`. Delink
applies the compiler's COFF decoration when it emits an object file.

```text
0x00413911,0x1C,func,xnew
```

The emitted COFF symbol is `_xnew`, which matches the name produced by x86
MSVC for a cdecl C function. cdecl is the default when `cc:` is omitted.

### Calling Conventions

Calling-convention attributes are intentionally removed. Store the exact
desired emitted symbol in the `Symbol` field when decoration matters. For
example:

```text
0x00413188,0x4F,func,_NT_handling_function@4
```

### Import Overrides

PE import names do not contain the source calling convention, so an import
whose compiler declaration is stdcall can be overridden by its IAT slot:

```text
0x004010BC,0x0,imp,__imp__SetConsoleCtrlHandler@8
```

Delink emits the corresponding import-pointer symbol:

```text
__imp__SetConsoleCtrlHandler@8
```

The `imp` symbol is emitted exactly as written. This is required when the
target uses a decorated import but the PE import table only exposes the
undecorated API name.

### Adding and Correcting Symbols

Existing function and object entries can be renamed or corrected by changing
the left-hand name or the `size:` attribute. A new `type:function` entry adds
an inferred function to the emitted object. A new `type:object` entry adds a
data symbol to relocation resolution; assign its byte range to a TU in
`splits.txt` when the object itself must contain that data.

After editing this file, normal Ninja builds automatically rerun delink and
propagate the changes. A function or object is emitted only when its range is
assigned to a TU in `splits.txt`; symbols not assigned to a split are not
emitted as fallback per-function objects. The explicit analysis target is also available:

```text
ninja analyze
```
