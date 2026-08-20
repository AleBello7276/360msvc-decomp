#!/usr/bin/env python3
"""Split one binary with delink from the config manifests.

Delink runs in config-driven mode (no x86 analysis): symbols/splits and the
emitted objects come purely from the authoritative `symbols.csv` / `splits.txt`
in config. The config files are copied to the build output so the build mirrors
them exactly. The analysis pass is only run on demand — `ninja analyze` (see
tools/bootstrap_manifests.py) — to re-seed those config files.
"""

from __future__ import annotations

import argparse
import shutil
import subprocess
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--symbols", type=Path, required=True)
    parser.add_argument("--splits", type=Path, required=True)
    parser.add_argument("--config-dir", type=Path, required=True)
    parser.add_argument("--objects", type=Path, required=True)
    parser.add_argument("--binary", required=True)
    parser.add_argument("--delink", type=Path, required=True)
    args = parser.parse_args()

    if args.out_dir.exists():
        shutil.rmtree(args.out_dir)
    args.out_dir.mkdir(parents=True, exist_ok=True)

    subprocess.run(
        [
            str(args.delink), "pe-split", str(args.input), "--outdir", str(args.out_dir),
            "--symbols", str(args.symbols), "--splits", str(args.splits),
        ],
        check=True,
    )

    # Mirror the editable config manifests over the build output so the build
    # CSV/splits are exactly what the user has in config. If a manifest is not
    # configured yet, delink still emitted something usable out of thin air.
    for name in ("symbols.csv", "splits.txt"):
        source = args.config_dir / name
        if source.is_file():
            shutil.copyfile(source, args.out_dir / name)


if __name__ == "__main__":
    main()
