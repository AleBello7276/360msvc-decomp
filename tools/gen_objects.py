#!/usr/bin/env python3
"""Generate one binary's objects.json from delink splits.txt."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def split_units(path: Path) -> list[str]:
    units: list[str] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if line.endswith(":") and not line.startswith(("Sections", ".")):
            units.append(line[:-1])
    return units


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--split", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--binary", required=True)
    parser.add_argument("--mw-version", default="X360/16.00.11886.00")
    parser.add_argument("--cflags", default="base")
    args = parser.parse_args()

    existing = json.loads(args.out.read_text(encoding="utf-8")) if args.out.exists() else {}
    old_statuses = {
        source: value if isinstance(value, str) else value.get("status", "MISSING")
        for library in existing.values()
        for source, value in library.get("objects", {}).items()
    }
    objects = {}
    for unit in split_units(args.split):
        stem = Path(unit).stem
        source_name = f"{stem}.c" if (Path("src") / args.binary / f"{stem}.c").is_file() else f"{stem}.cpp"
        source = str(Path(args.binary) / source_name)
        objects[source] = old_statuses.get(source, "MISSING")

    result = {
        args.binary: {
            "progress_category": args.binary,
            "mw_version": args.mw_version,
            "cflags": args.cflags,
            "objects": objects,
        }
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=4) + "\n", encoding="utf-8")
    print(f"Wrote {args.out}: {len(objects)} units")


if __name__ == "__main__":
    main()
