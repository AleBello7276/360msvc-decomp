#!/usr/bin/env python3
"""Seed editable per-binary manifests from a delink build output."""

from __future__ import annotations

import argparse
import re
import shutil
from pathlib import Path


GENERATED_GROUP = re.compile(r"^(?:\d{4}_|__)")


def editable_splits(text: str) -> str:
    lines = text.splitlines()
    output: list[str] = []
    keep = True
    for line in lines:
        stripped = line.strip()
        if not stripped or stripped == "Sections:":
            output.append(line)
            continue
        if not line.startswith(" ") and stripped.endswith(":"):
            keep = not GENERATED_GROUP.match(stripped[:-1])
        if keep:
            output.append(line)
    return "\n".join(output).rstrip() + "\n"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--config-dir", type=Path, required=True)
    args = parser.parse_args()

    args.config_dir.mkdir(parents=True, exist_ok=True)
    for name in ("symbols.txt", "splits.txt"):
        source = args.build_dir / name
        destination = args.config_dir / name
        if source.is_file() and not destination.exists():
            if name == "splits.txt":
                destination.write_text(editable_splits(source.read_text(encoding="utf-8")), encoding="utf-8")
            else:
                shutil.copyfile(source, destination)


if __name__ == "__main__":
    main()
