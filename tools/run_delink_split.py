#!/usr/bin/env python3
"""Run delink and refresh one binary's editable split manifests."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
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
    subprocess.run(
        [
            sys.executable, "tools/bootstrap_manifests.py",
            "--build-dir", str(args.out_dir), "--config-dir", str(args.config_dir),
        ],
        check=True,
    )


if __name__ == "__main__":
    main()
