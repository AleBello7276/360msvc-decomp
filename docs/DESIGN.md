# delink x86 Decomp Design (360msvc-decomp)

## 1. Objective

Decompile the seven **Microsoft Xbox 360 MSVC 16.00.11886.00** host compiler
binaries (the **Target**) located in `orig/16.00.11886.00/`:

| binary     | role                   |
|------------|------------------------|
| `cl.exe`   | driver (first target) |
| `c1.dll`   | front-end (C)         |
| `c1xx.dll` | front-end (C++)       |
| `c2.dll`   | optimizer / backend   |
| `link.exe` | linker                |
| `ml.exe`   | MASM assembler        |
| `mspdb100.dll` | program database runtime |

We rebuild C/C++ sources under `src/` using the **Compiler**
(`Compiler/`, same 16.00.11886.00 tools), split each Target PE into COFF
"target objects" with **delink**, and compare them with **objdiff**.

Concretely, per binary:

```
delink pe-split  (x86 analysis: functions + .data/.rdata strings & globals, no PDB)
    └─► symbols.txt      editable:  name = .text:0xADDR; // type size scope data
        └─► splits.txt   editable:  obj :  .text start end   (object contribution)
        └─► per-object .obj  = target objects          (grouping from splits.txt)
        └─► objects.json  initial MISSING entries derived from splits.txt

Compiler cl.exe  src/<path>.cpp ──► build/<ver>/<path>.obj   (built objects)

objdiff.json  units:
    target_path = <delink .obj>
    base_path   = <built .obj>
        │
    objdiff-cli report ──► per-function match %, folders per binary
```

## 2. Design decisions

1. **No PDB.** The matching PDBs in `Compiler/pdbs_ignore/` are deliberately
   ignored (folder is named `ignore`). delink's PDB-backed `pe-split` is not
   the path we use.
2. **No IDA dependency.** We add a lightweight **x86 analysis** to delink for
   the initial pass. It infers *function addresses and sizes* **and** the
   `.rdata` / `.data` / `.bss` layout. It needs no pseudo-code and does not
   need to be perfect; mistakes are corrected manually as they are found (that
   is the point of the editable `symbols.txt` / `splits.txt`). IDA remains an
   **optional fallback** because delink already has an `ida-split` workflow.
3. **First target: `cl.exe`.** All seven binaries are wired as separate objdiff
   "folders" from the start so the tooling is exercised end-to-end on all of
   them, but matching work begins on `cl.exe`.
4. **Gradual mapping.** `symbols.txt` (function + global data symbols) and
   `splits.txt` (object contribution = which byte ranges belong to which `.obj`)
   are the editable contract with delink, mirroring the dtk/jeff experience.

## 3. Repo reconnaissance (findings)

- **Target**: `360msvc-decomp/orig/16.00.11886.00/` — 32-bit x86 PE host
  binaries (the X360 compiler executables/DLLs).
- **PDBs**: present only under top-level `F:\msvc-decomp\Compiler\pdbs_ignore\`
  → ignored per decision above.
- **delink** (`delink/`, Rust):
  - `pe-split <exe> --pdb <pdb> -o outdir` → one COFF `.obj` per PDB module +
    `__shared_data.obj` (`crates/delink-cli/src/main.rs:989`). PDB-only, so not
    usable directly for us.
  - `ida-split <json> <binary> -o outdir` → one object per function (or per
    `idapro.json` group) + `__shared_data.obj` (`main.rs:1093`). Already uses
    **iced-x86** to disassemble x86/x86-64 and recover rel32/RIP-relative
    relocations; writes an editable `idapro.json` grouping.
  - Mach-O path writes editable `symtab.json` and a `manifest.json`; the PE
    path currently has **no** editable symbols/splits equivalent and **no**
    non-PDB analysis fallback.
  - `crates/delink-x86`, `delink-x86_64`, `delink-ida/src/emit.rs`,
    `idapro_json.rs`, `resolver.rs` are the pieces to reuse/extend.
- **Templates** — two dtk-derived `tools/project.py` in the tree:
  - `360msvc-decomp/tools/project.py` = base **MWCC/MWLD (GC/Wii)** template.
    No MSVC rules, no split rule, placeholder config.
  - `xkrnl-decomp/tools/project.py` = **jeff-template** fork — the reference.
    Has `cl.exe` (`/nologo /c ... /showIncludes /Fo$out $in`, `deps="msvc"`
    with prefix `Note: including file:`), `ml.exe` MASM, a `split` ninja rule,
    `config.yml` (`object`/`pdb`/`symbols`/`splits` + analysis flags), a
    `split → build/config.json (units)` step, `gen_objects.py` →
    `objects.json`, and objdiff maps
    `X360/16.00.11886.00 → msvc_ppc_16.00.11886.00` / `xbox360`
    (`project.py:1534`, `:1542`).

## 4. delink feature: x86 analysis (functions + data) with symbol/split manifest

### 4.1 Subcommand

Prefer extending the existing splitter rather than adding an unrelated command.
Two input modes for `pe-split`, chosen at runtime:

- **PDB mode** (existing) — require `--pdb`.
- **Analysis mode** (new) — no PDB. Set by config.yml `autosplit: true`
  (below). Runs the x86 analyser, then splits like the IDA path.

API shape (in `delink-ida/`-style):

```text
delink pe-split <pe>                  # analysis mode
    [--symbols <symbols.txt override>]
    [--splits <splits.txt override>]
    [--outdir <dir>]                  # objs + __shared_data.obj + symbols.txt + splits.txt
```

### 4.2 The analysis — inferred artifacts

#### Functions (`.text`)

Reuse `iced-x86` (already a dependency). Scan `.text` and produce a candidate
function-start set, then expand to fixed point:

- **Roots**
  - entry point / exports
  - direct call/jump **targets** from already-known code (`E8`/`E9`, `FF /2`, `FF /4`)
  - import-thunk addresses (IAT-jump stubs)
  - `.pdata`-style records if present (x64 only; x86 host has none)
- **Terminators**: `C3 ret`, `C2 imm16 ret`, `EB/E9` unconditional jump to an
  end / another function, `int3` (`CC`) padding, next known start.
- **Padding heuristic**: runs of `CC` (or `90` NOP / alignment fill) delimit
  functions — the byte after the padding run is a candidate start.
- **Iterate** until no new starts; walk each start forward to its terminator to
  get **size**.
- **Tolerance**: unresolved/tail calls and jumptables may leave wrapped or
  over-long functions; acceptable for the initial pass. The user fixes
  boundaries by editing `splits.txt`/`symbols.txt` and re-running.

#### Data (`.rdata`, `.data`, `.bss`)

dtk-style layout inference, applied after (and informed by) the code pass:

- **Strings**: scan `.rdata` and `.data` for printable null-terminated runs
  (ASCII and UTF-16/wide). Emit `type:object ... data:string|wstring` with the
  aligned size. Merge consecutive runs into a `string_table` where contiguous.
- **Globals & sizes**: break each data section into spans at alignment
  boundaries / padding `00` runs (mostly for `.bss`, which is all zeros) and at known references:
  - a value in the span that matches another known span's address (or a
    relocation target) marks a pointer-sized object of 4 bytes (x86);
  - consecutive zero/address runs delimit distinct globals; each global's
    **size** = span to the next global / alignment / end-of-section.
  - references from code (`lea`/`mov` into `.rdata`/`.data`, or relocations)
    become the canonical names/sizes; the rest are heuristic spans.
- Represent each inferred global as `type:object size:0xN`, with `data:` hint
  (`string`, `wstring`, `4byte`, `8byte`, `float`, `double`, `int`, …) where
  confidently known.
- **Not perfect by design**: over/under-sized globals and missed strings are
  expected and hand-fixed in `symbols.txt`.

### 4.3 Outputs

1. **`symbols.txt`** (dtk format). One line per function **and** per
   data global/string:
   ```
   fn_16001123 = .text:0x16001123; // type:function size:0x37 scope:global align:1
   Str_16017A34 = .rdata:0x16017A34; // type:object size:0x14 scope:global data:string
   ```
   Names are initially `fn_<addr>` / `str_<addr>` / `data_<addr>`; the user
   renames to real names as sources are reverse-engineered.
   **Decoration policy (x86 host).** `symbols.txt` stores the **clean
   undecorated name** (`foo`), with an optional `cc:` attribute
   (`cc:cdecl`/`cc:stdcall`/`cc:fastcall`; **default `cc:cdecl`**). delink
   applies the x86 decoration at COFF-emit time so the emitted symbol equals
   what `cl.exe` will emit: `foo`→`_foo`, `cc:stdcall` `foo`→`_foo@N` (N from a
   `cc_args`/bytes attr), `cc:fastcall` `foo`→`@foo@N`, and `?…@@…` C++ /
   `__imp_…` / `__real@…` names pass through verbatim. Matching in objdiff is
   then exact (`_foo`==`_foo`) with **zero `symbol_mappings`** — verified
   against `bluisblu/pvz` (x86 MSVC8 + delink `pe-split --pdb` + objdiff),
   whose PDB-carried names also match cl.exe output directly. Our no-PDB
   analyser cannot recover full C++ manglings, so C++-heavy TUs start from
   `fn_<addr>` placeholders and are hand-decorated.
   **objdiff fork for display.** objdiff's demangler only undecorates
   `?`-mangled C++ names (never `_foo`), so we maintain a small **objdiff
   fork** that undecorates x86 C symbols for display like MSVC `undname`:
   `_foo`→`cdecl foo`, `_foo@8`→`stdcall foo`, `@foo@8`→`fastcall foo`,
   `?…`→existing C++ demangle. This is display-only; matching stays exact.
   `symbol_mappings` remains only as an escape hatch.
2. **`splits.txt`** (dtk format). Header:
   ```
   Sections:
       .text      type:code  align:16
       .rdata     type:rodata align:16
       .data      type:data  align:16
       .bss       type:bss   align:16
   ```
   and per output object the exact spans it owns — this is the **object
   contribution** map:
   ```
   cl/driver.obj:
       .text   start:0x16001000 end:0x16001200
       .rdata  start:0x16017A34 end:0x16017A80
   ```
   Default grouping = **one object per function** (best for objdiff), with
   adjacent data grouped into a per-binary `__data.obj` (or folded into the
   same object as the referencing function). Editing these spans lets the user
   merge functions AND their data into per-source-file `.obj`s.
3. **`__shared_data.obj`** and per-unit COFF `.obj`s, identical to the IDA
   path. No `config.json`-units artifact: the object list for the project comes
   from `splits.txt` (§5.4).

### 4.4 Round-trip / re-run

On re-run, if `--symbols`/`--splits` are provided (or found next to the inputs
and newer), delink:
- renames symbols per edited `symbols.txt`,
- merges/splits objects per edited `splits.txt`,
- only recomputes boundaries for gaps the user left unedited.

This mirrors `idapro.json`/`symtab.json` and is the core "gradually map
symbols + objects" experience.

### 4.5 Format decision: `symbols.txt` / `splits.txt`

**Recommendation: keep the human-editable dtk text formats for both, and emit
an optional sidecar JSON for strict machine consumption.**

- **`symbols.txt` → keep the dtk line format** (not CSV):
  `name = section:addr; // key:value ...`
  - MSVC mangled names contain characters (`?`, `$`, `@`, `.`, backslashes,
    occasionally `,` inside template/`__FUNCDNAME__` strings) that sit fine in
    this `name = …; // attrs` grammar but are fragile in strict CSV (quoting,
    inline `;` comments). A `;`-led comment would also have to be dropped.
  - It is the established format the borrowed dtk/jeff tooling already parses,
    so we keep interop for free.
  - CSV buys spreadsheet editing, but symbol lists are usually far more than a
    row-per-symbol edit; the header is also where dtk stores arbitrary
    per-symbol attributes (`data:`, `scope:`, `align:`, `force_active`,
    `noreloc`) that don't map to a fixed CSV column.
- **`splits.txt` → keep the structured text format** (not CSV). The data is
  inherently **hierarchical** (`file.obj:` → many `section start end [attr]`
  rows), so a flat CSV table is a worse fit and less readable than the
  indented block format.
- **Machine-readable canonical form**: delink additionally writes
  `symbols.json` / `splits.json` (and a `manifest.json`) capturing the same
  contents structurally. Tools and CI read the JSON; humans edit the *txt*.
  On re-run delink parses the newest of the two and writes both back, keeping
  them in sync — this mirrors how delink already treats `idapro.json` /
  `symtab.json` as the editable JSON contract.

If you'd still prefer CSV for symbols for sake of spreadsheets, that's a
one-function change in the delink emitter (there is no downstream dependency on
the file suffix); but I'd keep the dtk format for interop + comments unless you
specifically need tabular editing.

## 5. Template integration (360msvc-decomp)

### 5.0 Config file roles

Three user-edited config files (plus the generated `objdiff.json`) have
distinct jobs — mirrored from dtk/jeff so the names stay familiar:

| file | role | authored by |
|------|------|-------------|
| `config/<ver>/config.yml` | **splitter / project** config: all binaries/modules and their `symbols:`/`splits:` paths plus `autosplit` flags. delink reads this. | user |
| `config/<ver>/config.json` | **progress categories + compiler-flag categories** (used for progress report and the decomp.me/scratch flags). **Not** the object list. | user |
| `config/<ver>/<binary>/objects.json` | the **modules/object list** for one binary: every translation unit / source file with a progress tag (`MISSING`, `Matching`, `NonMatching`, `Equivalent`, `LinkIssues`), plus per-lib `mw_version` and a `cflags` category. | user, bootstrap via `gen_objects.py` |
| `objdiff.json` (generated) | pairs each target `.obj` with its built `.obj` for objdiff | `tools/project.py` |

`config.json` (categories+flags) and `objects.json` (units+statuses) are the
two the user called "a little ambiguous"; the rule of thumb is: **config.json
= how to compile, objects.json = what to compile and whose progress.**

### 5.1 `config/<ver>/` layout

Shared version metadata stays at the version root. Each target binary gets an
isolated directory so its symbols, splits, and object statuses cannot collide:

```text
config/16.00.11886.00/
  config.json             # shared progress categories and compiler flags
  config.example.yml
  config.yml               # shared splitter/project config for all binaries
  cl/       # symbols.txt, splits.txt, objects.json
  c1/       # symbols.txt, splits.txt, objects.json
  c1xx/     # symbols.txt, splits.txt, objects.json
  c2/       # symbols.txt, splits.txt, objects.json
  link/     # symbols.txt, splits.txt, objects.json
  ml/       # symbols.txt, splits.txt, objects.json
  mspdb100/ # symbols.txt, splits.txt, objects.json when available
```

`config.json` remains shared because its categories and compiler flags span all
targets. `objects.json` is per binary because its units and statuses are not
shared. Generated target objects use matching build subdirectories, and
`objdiff.json` remains at the version/build root with binary-prefixed names.

### 5.2 Shared `config.yml`

Adopt the jeff format, minus PDB:

```yaml
object: orig/16.00.11886.00/cl.exe
symbols: config/16.00.11886.00/cl/symbols.txt
splits: config/16.00.11886.00/cl/splits.txt
autosplit: true        # analysis mode (no PDB) -> delink pe-split analysis path
autosplit_symbols: false  # fill symbols from analysis only (fn_<addr>)
autosplit_splits: false   # one-obj-per-function default
```

Seven modules (one YAML `modules:` entry each) so objdiff shows a folder per
`c1/cl/c1xx/c2/link/ml/mspdb100`.

### 5.3 `tools/project.py`

Port the jeff-template logic into the 360msvc `project.py`:
- `msvc` rule: `{cl.exe} $cflags /showIncludes /Fo$out $in`, `deps="msvc"`,
  prefix `Note: including file:`.
- `masm` rule for `ml.exe`.
- A `delink` split rule:
  ```
   delink pe-split $in --outdir build/<ver>/<binary> ...
  ```
   producing per-binary `symbols.txt` / `splits.txt` and target `.obj`s in
   `build/<ver>/<binary>/` — replaces the `dtk xex split` edge from
  `xkrnl-decomp/tools/project.py:1400` (that one emits a config.json of units;
  ours doesn't need it, see §5.0).
- objdiff unit generation (`generate_objdiff_config`) driven by `objects.json`
  + the object list parsed from `splits.txt`, mirroring
  `xkrnl-decomp/tools/project.py:1461`.
- Scratch/maps: for local matching the objdiff **architecture comes from the
  COFF files**; the `X360/… → msvc_ppc_…` decomp.me-only map is not required
  for local matching but is retained for `scratch` submit support.

### 5.4 objdiff grouping (folders per binary)

- `progress_categories`: one per binary (`cl`, `c1`, `c1xx`, `c2`, `link`,
  `ml`, `mspdb100`).
- Unit names are prefixed by binary, e.g. `cl/driver.obj`, so objdiff renders a
  folder per target binary.

### 5.5 `tools/gen_objects.py`

Port from `xkrnl-decomp/gen_objects.py` but source the unit list from delink's
**`splits.txt`** (each `path/name.obj:` header is a translation unit / object)
rather than a config.json-of-units. Emit each binary's
`config/<ver>/<binary>/objects.json` with every object `"MISSING"`, preserving
existing statuses.

### 5.6 `configure.py` / `tools/defines_common.py`

- Load shared `config/<ver>/config.json` (categories + flag categories) and
  each binary's `config/<ver>/<binary>/objects.json` exactly like the jeff-template
  (`xkrnl-decomp/configure.py:184`, `:258`).
- `VERSIONS = ["16.00.11886.00"]` in `tools/defines_common.py`.
- Compilers path → `Compiler/`; `config.linker_version = "X360/16.00.11886.00"`.

## 6. objdiff config

```jsonc
{
  "min_version": "2.0.0-beta.5",
  "custom_make": "ninja",
  "build_target": false,
  "units": [
    {
      "name": "cl/driver.obj",
      "target_path": "build/16.00.11886.00/cl/driver.obj",
      "base_path": "build/16.00.11886.00/objects/driver.obj",
      "metadata": { "source_path": "src/cl/driver.c", "progress_categories": ["cl"] },
      "symbol_mappings": null
    }
  ],
  "progress_categories": [ { "id": "cl", "name": "cl.exe" }, ... ]
}
```

`symbol_mappings` are preserved across reconfigures (existing jeff behaviour).

## 7. Milestones

1. **M1 – delink analyser**: `pe-split` analysis mode emits `symbols.txt`
   (functions + strings/globals), `splits.txt`, and target `.obj`s for
   `cl.exe`. Validate: objdiff opens the `cl.exe` folder, shows N functions
   with addresses and the inferred data symbols.
2. **M2 – template wiring**: config roles (§5.0), MSVC rules + split rule +
   `gen_objects.py` + objdiff config in 360msvc-decomp. `ninja` builds a source
   object and objdiff compares it.
3. **M3 – seven binaries**: same pipeline for all seven; objdiff folders
   populated.
4. **M4 – symbol mapping loop**: edit `symbols.txt`/`splits.txt`, re-run
   analysis, confirm delink honors renames/merges and objdiff reflects them.
5. **M5 – data refinement**: tighten string detection, pointer/global sizing,
   and per-object data grouping, plus manual re-annotation of leftover gaps.

## 8. Risks & open questions

### 8.1 Architecture of the rebuilt host binaries
The Target binaries are **x86 host** (32-bit). The Compiler's `cl.exe` is the
X360 (PPC-emitting) compiler. Confirm which host toolchain compiles the
rebuilt x86 CRT/driver code so the built objs match the x86 target objs. For
objdiff **local** matching this matters only for the architecture field, but
object layout/flags must be coherent x86 COFF — verify `Compiler/cl.exe` can
emit x86 host objects (or that a host MSVC 2010 `cl.exe` is intended for the
rebuild). *Assumed: yes per the "same compiler that built the Target" premise.*

### 8.2 Data-symbol coverage
Data inference (strings/globals/sizes) is **in scope for M1** but heuristic:
over/under-sized globals, merged or missed strings, and mis-grouped data are
expected. §4.2's needs a coarse pass first (strings + alignment/pointer spans),
refined in M5. Users can hand-add/override any entry in `symbols.txt` at any
time.

### 8.3 Analysis quality
x86 host binaries contain import thunks, TLS, data that looks like code, and
jumptable-heavy switch code. Expect over/under-bounded functions until the
iterative analyser matures or manual edits land. This is accepted for the
initial pass.

### 8.4 IDA fallback
`ida-split` already delivers the same manifest shape if the analyser
underperforms early. The template's `autosplit` flag can select IDA-import mode
later without changing `project.py`/objdiff.

## 9. Repo layout (target end state)

```
360msvc-decomp/
  orig/16.00.11886.00/            # Target binaries (existing)
  src/<binary>/<file>.c/.cpp      # decompiled sources (new)
  Compiler/                       # rebuild toolchain (existing)
  config/16.00.11886.00/
     config.yml                   # object/symbols/splits/autosplit
     symbols.txt                  # editable
     splits.txt                   # editable
     objects.json                 # ported statuses (gen_objects.py)
  build/16.00.11886.00/           # generated: target objs, built objs, symbols/splits + JSON sidecars
  update/                         # objdiff-cli report output (optional)
```
