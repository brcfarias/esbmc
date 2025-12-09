#!/usr/bin/env python3
"""
Lightweight performance harness for ESBMC.

It reads a JSON case list, runs each case with /usr/bin/time -v, and emits
aggregate metrics (medians) to JSON/CSV. Intended for CI/cron usage.
"""

from __future__ import annotations

import argparse
import csv
import datetime as dt
import json
import pathlib
import re
import statistics
import subprocess
import sys
import time
from typing import Any, Dict, List, Optional


TIME_PATTERN_MAP = {
    "user_sec": re.compile(r"User time \(seconds\):\s*([0-9.]+)"),
    "sys_sec": re.compile(r"System time \(seconds\):\s*([0-9.]+)"),
    "max_rss_kb": re.compile(
        r"Maximum resident set size \(kbytes\):\s*([0-9]+)"
    ),
}

EXTRA_PATTERNS = {
    "encode_time_s": [
        re.compile(r"Encoding to solver time:\s*([0-9.]+)s", re.IGNORECASE)
    ],
    "solver_time_s": [
        re.compile(r"SMT solving time:\s*([0-9.]+)s", re.IGNORECASE),
        re.compile(r"Solver time:\s*([0-9.]+)s", re.IGNORECASE),
        re.compile(
            r"Runtime decision procedure:\s*([0-9.]+)s", re.IGNORECASE
        ),
    ],
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "cases",
        nargs="?",
        default="benchmarks/perf_cases.json",
        help="Path to JSON file listing benchmark cases",
    )
    parser.add_argument(
        "--esbmc-bin",
        default="build/src/esbmc/esbmc",
        help="Path to the ESBMC binary to run",
    )
    parser.add_argument(
        "--repeat",
        type=int,
        default=1,
        help="How many times to run each case (median is reported)",
    )
    parser.add_argument(
        "--out-json",
        default="perf_results.json",
        help="Where to write aggregated metrics (JSON)",
    )
    parser.add_argument(
        "--out-csv",
        default="perf_results.csv",
        help="Where to write aggregated metrics (CSV)",
    )
    parser.add_argument(
        "--log-dir",
        default=None,
        help="If set, save stdout/stderr for each run in this directory",
    )
    parser.add_argument(
        "--time-bin",
        default="/usr/bin/time",
        help="Path to GNU time; if missing, wall clock is still recorded",
    )
    parser.add_argument(
        "--workdir",
        default=".",
        help="Working directory to run benchmarks from",
    )
    return parser.parse_args()


def load_cases(path: pathlib.Path) -> List[Dict[str, Any]]:
    with path.open("r", encoding="utf-8") as fh:
        data = json.load(fh)
    if not isinstance(data, list):
        raise ValueError("Case file must contain a list")
    for item in data:
        if "name" not in item or "path" not in item:
            raise ValueError("Each case needs at least 'name' and 'path'")
    return data


def parse_time_output(stderr: str) -> Dict[str, Optional[float]]:
    metrics: Dict[str, Optional[float]] = {k: None for k in TIME_PATTERN_MAP}
    for key, pattern in TIME_PATTERN_MAP.items():
        m = pattern.search(stderr)
        if m:
            metrics[key] = float(m.group(1))
    return metrics


def parse_extra_metrics(output: str) -> Dict[str, Optional[float]]:
    metrics: Dict[str, Optional[float]] = {k: None for k in EXTRA_PATTERNS}
    for key, patterns in EXTRA_PATTERNS.items():
        for pattern in patterns:
            match = pattern.search(output)
            if match:
                metrics[key] = float(match.group(1))
                break
    return metrics


def median_or_none(values: List[Optional[float]]) -> Optional[float]:
    numeric = [v for v in values if v is not None]
    if not numeric:
        return None
    return statistics.median(numeric)


def run_case(
    case: Dict[str, Any],
    args: argparse.Namespace,
    root: pathlib.Path,
    log_dir: Optional[pathlib.Path],
) -> Dict[str, Any]:
    case_path = root / case["path"]
    timeout = int(case.get("timeout", 60))
    cmd = [args.esbmc_bin, str(case_path)]
    cmd.extend(case.get("args", []))

    runs: List[Dict[str, Any]] = []
    for idx in range(args.repeat):
        final_cmd = cmd
        use_time = pathlib.Path(args.time_bin).exists()
        if use_time:
            final_cmd = [args.time_bin, "-v"] + cmd

        start = time.perf_counter()
        status = "ok"
        stdout = ""
        stderr = ""
        returncode: Optional[int] = None

        try:
            proc = subprocess.run(
                final_cmd,
                cwd=root,
                capture_output=True,
                text=True,
                timeout=timeout,
                check=False,
            )
            stdout = proc.stdout or ""
            stderr = proc.stderr or ""
            returncode = proc.returncode
            if proc.returncode != 0:
                status = "error"
        except subprocess.TimeoutExpired as exc:
            stdout = exc.stdout or ""
            stderr = exc.stderr or ""
            status = "timeout"

        wall_ms = (time.perf_counter() - start) * 1000.0

        time_metrics = parse_time_output(stderr if use_time else "")
        extras = parse_extra_metrics(stdout + "\n" + stderr)

        run_record: Dict[str, Any] = {
          "status": status,
          "returncode": returncode,
          "wall_ms": wall_ms,
          "user_sec": time_metrics["user_sec"],
          "sys_sec": time_metrics["sys_sec"],
          "max_rss_kb": time_metrics["max_rss_kb"],
          "encode_time_s": extras["encode_time_s"],
          "solver_time_s": extras["solver_time_s"],
        }

        if log_dir:
            log_dir.mkdir(parents=True, exist_ok=True)
            log_file = log_dir / f"{case['name']}-run{idx+1}.log"
            log_file.write_text(stdout + "\n" + stderr, encoding="utf-8")

        runs.append(run_record)

    return {"name": case["name"], "cmd": " ".join(cmd), "runs": runs}


def aggregate_case(case_result: Dict[str, Any]) -> Dict[str, Any]:
    runs = case_result["runs"]
    agg: Dict[str, Any] = {"name": case_result["name"], "cmd": case_result["cmd"]}

    agg["status"] = "ok"
    if any(r["status"] == "timeout" for r in runs):
        agg["status"] = "timeout"
    elif any(r["status"] == "error" for r in runs):
        agg["status"] = "error"

    for key in ["wall_ms", "user_sec", "sys_sec", "max_rss_kb", "encode_time_s", "solver_time_s"]:
        agg[key] = median_or_none([r.get(key) for r in runs])

    case_result["aggregate"] = agg
    return case_result


def write_json(results: Dict[str, Any], path: pathlib.Path) -> None:
    path.write_text(json.dumps(results, indent=2), encoding="utf-8")


def write_csv(results: Dict[str, Any], path: pathlib.Path) -> None:
    headers = [
        "name",
        "cmd",
        "status",
        "wall_ms",
        "user_sec",
        "sys_sec",
        "max_rss_kb",
        "encode_time_s",
        "solver_time_s",
    ]
    with path.open("w", newline="", encoding="utf-8") as fh:
        writer = csv.DictWriter(fh, fieldnames=headers)
        writer.writeheader()
        for case in results["cases"]:
            agg = dict(case["aggregate"])
            # Ensure only declared headers are written
            writer.writerow({h: agg.get(h) for h in headers})


def main() -> int:
    args = parse_args()
    root = pathlib.Path(args.workdir).resolve()
    esbmc_bin = pathlib.Path(args.esbmc_bin)
    if not esbmc_bin.exists():
        print(f"ESBMC binary not found: {esbmc_bin}", file=sys.stderr)
        return 1

    cases = load_cases(pathlib.Path(args.cases))
    log_dir = pathlib.Path(args.log_dir) if args.log_dir else None

    case_results: List[Dict[str, Any]] = []
    for case in cases:
        case_results.append(run_case(case, args, root, log_dir))

    aggregated = [aggregate_case(r) for r in case_results]
    payload = {
        "esbmc_bin": str(esbmc_bin),
        "generated_at": dt.datetime.now(dt.timezone.utc).isoformat(),
        "repeat": args.repeat,
        "cases": aggregated,
    }

    write_json(payload, pathlib.Path(args.out_json))
    write_csv(payload, pathlib.Path(args.out_csv))
    print(f"Wrote {args.out_json} and {args.out_csv}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
