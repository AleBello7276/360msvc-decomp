#!/usr/bin/env python3
"""Remove generated version artifacts while preserving downloaded tools."""

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("version", type=str)
    parser.add_argument("--build-dir", type=Path, default=Path("build"))
    args = parser.parse_args()

    version_dir = args.build_dir / args.version
    if not version_dir.is_dir():
        return
    for path in version_dir.iterdir():
        shutil.rmtree(path) if path.is_dir() else path.unlink()


if __name__ == "__main__":
    main()
