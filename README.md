# Xbox 360 MSVC Decompilation

## Required tools

get the following:

- xbox 360 compiler version 11886 binaries: `orig/16.00.11886.00/`
- MSVC toolchain and headers from VS2010: `Compiler/`
- delink binary: `build/tools/delink.exe`
- rust (to compile delink), ninja, python, windows or a way to run the compiler.


build delink with:

```text
cargo build --release --bin delink
```

Copy `delink/target/release/delink.exe` to
`360msvc-decomp/build/tools/delink.exe`.

## VS2010

too lazy to make instructions on how to get the stuff needed and the compiler
version. at some point i could make some repository or archive so they can be pulled with some script
