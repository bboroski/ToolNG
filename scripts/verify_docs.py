#!/usr/bin/env python3
"""Validate README local links and the committed SVG preview assets."""

from __future__ import annotations

import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]
README = ROOT / "README.md"
REQUIRED_SCREENS = (
    "assets/screenshots/overview.svg",
    "assets/screenshots/usb-devices.svg",
    "assets/screenshots/reports.svg",
)


def main() -> int:
    if not README.is_file():
        print("README.md is missing", file=sys.stderr)
        return 1

    content = README.read_text(encoding="utf-8")
    if r"\n\n" in content:
        print("README contains literal escaped newlines", file=sys.stderr)
        return 1

    targets: list[str] = []
    targets.extend(re.findall(r"!?\[[^\]]*\]\(([^)]+)\)", content))
    targets.extend(re.findall(r"<img\b[^>]*\bsrc=[\"']([^\"']+)[\"']", content, re.I))

    errors: list[str] = []
    checked = 0
    root_dir = ROOT.resolve()

    for raw in targets:
        target = raw.split()[0].strip("<>")
        if not target or target.startswith(("#", "https://", "http://", "mailto:")):
            continue
        path_part = unquote(target.split("#", 1)[0].split("?", 1)[0])
        if not path_part:
            continue
        resolved = (ROOT / path_part).resolve()
        if root_dir not in resolved.parents and resolved != root_dir:
            errors.append(f"path escapes repository: {raw}")
            continue
        if not resolved.is_file():
            errors.append(f"missing local target: {raw}")
            continue
        checked += 1
        if resolved.suffix.lower() == ".svg":
            try:
                svg_root = ET.parse(resolved).getroot()
                if svg_root.tag.rsplit("}", 1)[-1] != "svg":
                    errors.append(f"not an SVG root: {raw}")
                label = "".join(svg_root.itertext()).upper()
                if "SAMPLE DATA" not in label and "READme-HERO" not in str(svg_root.attrib).upper():
                    # The required screenshot previews must be visibly labelled as mock data.
                    if path_part in REQUIRED_SCREENS:
                        errors.append(f"sample-data label missing from {raw}")
            except ET.ParseError as exc:
                errors.append(f"invalid SVG {raw}: {exc}")

    for relative in REQUIRED_SCREENS:
        if not (ROOT / relative).is_file():
            errors.append(f"required screenshot preview missing: {relative}")

    if errors:
        print("\n".join(f"ERROR: {error}" for error in errors), file=sys.stderr)
        return 1

    print(f"README local targets validated: {checked}")
    print(f"UI preview SVGs validated: {len(REQUIRED_SCREENS)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
