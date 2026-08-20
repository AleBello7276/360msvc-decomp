#!/usr/bin/env python3
"""Run the delink x86 analysis for one binary (manual, on demand).

This is the only place the no-PDB x86 analysis runs. It writes raw analysis
manifests (functions, strings, splits) to an analysis subdirectory so they can
be reviewed and folded into the config manifests. Normal builds use
run_delink_split.py, which never runs the analysis.
"""

from __future__ import annotations

import argparse
import subprocess
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--delink", type=Path, required=True)
    args = parser.parse_args()

    args.out_dir.mkdir(parents=True, exist_ok=True)

    subprocess.run(
        [
            str(args.delink), "pe-split", str(args.input), "--outdir", str(args.out_dir),
            "--analyze",
        ],
        check=True,
    )


if __name__ == "__main__":
    main()