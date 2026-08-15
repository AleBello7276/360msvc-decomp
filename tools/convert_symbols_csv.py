#!/usr/bin/env python3
"""Convert legacy symbols.txt entries to symbols.csv."""

from __future__ import annotations

import re
import sys
from pathlib import Path


def convert(path: Path) -> None:
    rows = ["Address,Size,Type,Symbol"]
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line or line.lstrip().startswith(("#", "//")) or "=" not in line:
            continue
        name, definition = (part.strip() for part in line.split("=", 1))
        match = re.search(r"\.([A-Za-z0-9_]+):\s*(0x[0-9A-Fa-f]+)", definition)
        if not match:
            continue
        section, address = match.groups()
        comment = line.split("//", 1)[1] if "//" in line else ""
        size_match = re.search(r"size:0x([0-9A-Fa-f]+)", comment)
        size = f"0x{size_match.group(1)}" if size_match else "0x0"
        type_match = re.search(r"type:([A-Za-z]+)", comment)
        symbol_type = "imp" if type_match and type_match.group(1) == "import" else ("func" if type_match and type_match.group(1) == "function" else "data")
        if section == "idata":
            symbol_type = "imp"
        if symbol_type == "imp" and not name.startswith("__imp_"):
            cc = re.search(r"cc:([A-Za-z]+)", comment)
            args = re.search(r"cc_args:0x([0-9A-Fa-f]+)", comment)
            if cc and args and cc.group(1) == "stdcall":
                name = f"__imp__{name}@{int(args.group(1), 16)}"
            else:
                name = f"__imp_{name}"
        rows.append(f"{address},{size},{symbol_type},{name}")
    path.write_text("\n".join(rows) + "\n", encoding="utf-8")


for argument in sys.argv[1:]:
    convert(Path(argument))
