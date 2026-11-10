#!/usr/bin/env python3
"""One-knob build, test and Windows desktop launcher for ToolNG."""
from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
BUILD = ROOT / "build"


def run(label: str, command: list[str]) -> None:
    print(f"\n==> {label}", flush=True)
    subprocess.run(command, cwd=ROOT, check=True)


def check() -> None:
    cmake = shutil.which("cmake")
    if not cmake:
        raise RuntimeError("CMake is required. Install CMake and a C17-capable compiler.")
    run("Configure portable core and native desktop target", [cmake, "-S", ".", "-B", "build", "-DBUILD_TESTING=ON"])
    run("Build ToolNG and tests", [cmake, "--build", "build", "--config", "Release", "--parallel"])
    ctest = shutil.which("ctest") or "ctest"
    run("Run CTest suite", [ctest, "--test-dir", "build", "-C", "Release", "--output-on-failure"])
    verifier = ROOT / "scripts" / "verify_docs.py"
    if verifier.is_file():
        run("Verify documentation and previews", [sys.executable, str(verifier)])


def launch() -> None:
    if sys.platform != "win32":
        print("The native desktop shell is Win32-only. Portable core build and tests passed.")
        return
    candidates = (BUILD / "Release" / "ToolzNG.exe", BUILD / "ToolzNG.exe")
    executable = next((item for item in candidates if item.is_file()), None)
    if executable is None:
        raise RuntimeError("Build completed but ToolzNG.exe was not found.")
    print(f"\n==> Launch {executable}", flush=True)
    subprocess.Popen([str(executable)], cwd=ROOT)


def main() -> int:
    parser = argparse.ArgumentParser(description="ToolNG one-knob build, test and launcher.")
    parser.add_argument("command", nargs="?", choices=("check", "build", "run"), default="run")
    args = parser.parse_args()
    try:
        if args.command == "build":
            if not shutil.which("cmake"):
                raise RuntimeError("CMake is required.")
            run("Configure", ["cmake", "-S", ".", "-B", "build", "-DBUILD_TESTING=ON"])
            run("Build", ["cmake", "--build", "build", "--config", "Release", "--parallel"])
        elif args.command == "check":
            check()
        else:
            check()
            launch()
    except (subprocess.CalledProcessError, RuntimeError) as exc:
        print(f"ToolNG one-knob failed: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
