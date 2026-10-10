#!/usr/bin/env python3
"""Normalize per-file coverage and render the cross-platform job summary."""

from __future__ import annotations

import argparse
from collections import defaultdict
import glob
import json
import os
from pathlib import Path
import re
import sys
import xml.etree.ElementTree as ET


SCHEMA_VERSION = 2
LANE_ORDER = {
    ("Linux", "GCC"): 0,
    ("Linux", "Clang"): 1,
    ("macOS", "AppleClang"): 2,
    ("Windows", "MSVC"): 3,
    ("Windows", "clang-cl"): 4,
}
MARKER_PATTERN = re.compile(r"@coverage-target:\s*(\S+)")


def percentage(covered: int, total: int) -> float | None:
    return round(covered * 100.0 / total, 2) if total else None


def metric(covered: int, total: int) -> dict[str, int | float | None]:
    return {"covered": covered, "total": total, "percent": percentage(covered, total)}


def normalize_path(source: str, source_root: Path) -> str:
    normalized = source.replace("\\", "/")
    root = str(source_root.resolve()).replace("\\", "/").rstrip("/")
    if normalized.casefold().startswith((root + "/").casefold()):
        return normalized[len(root) + 1 :]
    # Also recognize reports produced in a different Actions workspace. This
    # keeps downloaded reports reproducible locally without embedding a runner
    # path in the normalized summary.
    for marker in ("/llbc/src/", "/llbc/include/"):
        index = normalized.casefold().rfind(marker.casefold())
        if index >= 0:
            return normalized[index + 1 :]
    return normalized


def coverage_targets(source_root: Path, markers_dir: Path) -> set[str]:
    patterns = set()
    for marker_file in markers_dir.rglob("*"):
        if marker_file.suffix not in (".cpp", ".h"):
            continue
        text = marker_file.read_text(encoding="utf-8", errors="replace")
        patterns.update(MARKER_PATTERN.findall(text))

    targets = set()
    for pattern in patterns:
        absolute_pattern = str(source_root / pattern)
        for matched in glob.glob(absolute_pattern):
            path = Path(matched)
            if path.is_file():
                targets.add(path.resolve().relative_to(source_root.resolve()).as_posix())
    if not targets:
        raise RuntimeError(f"no coverage targets found from markers under {markers_dir}")
    return targets


def parse_lcov(path: Path, source_root: Path) -> dict[str, dict[int, int]]:
    files: dict[str, dict[int, int]] = defaultdict(dict)
    current = ""
    for raw_line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if raw_line.startswith("SF:"):
            current = normalize_path(raw_line[3:], source_root)
        elif current and raw_line.startswith("DA:"):
            line_number, hits, *_ = raw_line[3:].split(",")
            number = int(line_number)
            files[current][number] = max(files[current].get(number, 0), int(hits))
    return files


def parse_cobertura(path: Path, source_root: Path) -> dict[str, dict[int, int]]:
    files: dict[str, dict[int, int]] = defaultdict(dict)
    root = ET.parse(path).getroot()
    for class_node in root.findall(".//class"):
        filename = class_node.get("filename")
        if not filename:
            continue
        source = normalize_path(filename, source_root)
        for line_node in class_node.findall("./lines/line"):
            number = int(line_node.get("number", "0"))
            hits = int(line_node.get("hits", "0"))
            files[source][number] = max(files[source].get(number, 0), hits)
    return files


def module_name(path: str) -> str:
    parts = Path(path).parts
    if len(parts) >= 3 and parts[:2] == ("llbc", "src"):
        return parts[2]
    if len(parts) >= 4 and parts[:3] == ("llbc", "include", "llbc"):
        return parts[3]
    return parts[0] if parts else "other"


def summarize_files(
    raw_files: dict[str, dict[int, int]], targets: set[str]
) -> tuple[list[dict], dict[str, int | float | None]]:
    files = []
    total_covered = 0
    total_lines = 0
    target_lookup = {target.casefold(): target for target in targets}
    for raw_path, lines in raw_files.items():
        target = target_lookup.get(raw_path.casefold())
        if target is None or not lines:
            continue
        covered = sum(hits > 0 for hits in lines.values())
        total = len(lines)
        files.append({"path": target, "module": module_name(target), "lines": metric(covered, total)})
        total_covered += covered
        total_lines += total
    files.sort(key=lambda item: item["path"])
    return files, metric(total_covered, total_lines)


def write_lane_summary(args: argparse.Namespace) -> None:
    report = Path(args.input)
    source_root = Path(args.source_root)
    markers_dir = Path(args.markers_dir)
    if not report.is_file():
        raise FileNotFoundError(f"coverage report not found: {report}")
    if args.format == "lcov":
        raw_files = parse_lcov(report, source_root)
    else:
        raw_files = parse_cobertura(report, source_root)

    files, lines = summarize_files(raw_files, coverage_targets(source_root, markers_dir))
    if lines["total"] == 0:
        raise RuntimeError(f"coverage report contains no targeted line data: {report}")
    payload = {
        "schema_version": SCHEMA_VERSION,
        "platform": args.platform,
        "compiler": args.compiler,
        "backend": args.backend,
        "lines": lines,
        "files": files,
    }
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(
        f"{args.platform} / {args.compiler}: {lines['percent']:.2f}% lines "
        f"across {len(files)} files"
    )


def load_summaries(input_dir: Path, expected_count: int) -> list[dict]:
    paths = sorted(input_dir.rglob("coverage-summary.json"))
    if len(paths) != expected_count:
        raise RuntimeError(
            f"expected {expected_count} coverage summaries under {input_dir}, found {len(paths)}"
        )
    summaries = []
    seen = set()
    for path in paths:
        data = json.loads(path.read_text(encoding="utf-8"))
        if data.get("schema_version") != SCHEMA_VERSION:
            raise RuntimeError(f"unsupported coverage summary schema in {path}")
        lane = (data["platform"], data["compiler"])
        if lane in seen:
            raise RuntimeError(f"duplicate coverage summary for {lane[0]} / {lane[1]}")
        seen.add(lane)
        summaries.append(data)
    return sorted(
        summaries,
        key=lambda item: (
            LANE_ORDER.get((item["platform"], item["compiler"]), 99),
            item["platform"],
            item["compiler"],
        ),
    )


def percent_cell(values: dict | None) -> str:
    if not values or values["percent"] is None:
        return "—"
    return f"{values['percent']:.2f}%"


def module_metrics(paths: list[str], lane_files: dict[str, dict]) -> dict:
    covered = sum(lane_files[path]["covered"] for path in paths if path in lane_files)
    total = sum(lane_files[path]["total"] for path in paths if path in lane_files)
    return metric(covered, total)


def render_markdown(summaries: list[dict]) -> str:
    lane_names = [f"{item['platform']} / {item['compiler']}" for item in summaries]
    lane_maps = [
        {entry["path"]: entry["lines"] for entry in item["files"]}
        for item in summaries
    ]
    modules: dict[str, set[str]] = defaultdict(set)
    for item in summaries:
        for entry in item["files"]:
            modules[entry["module"]].add(entry["path"])

    lines = [
        "## Coverage by file / module",
        "",
        "Only line coverage is shown. Expand a module to inspect individual files.",
        "",
        "| Platform / compiler | Lines |",
        "| --- | ---: |",
    ]
    for item in summaries:
        lines.append(f"| {item['platform']} / {item['compiler']} | {percent_cell(item['lines'])} |")

    lines.extend(["", "### Modules and files", ""])
    header = "| File | " + " | ".join(lane_names) + " |"
    separator = "| --- | " + " | ".join("---:" for _ in summaries) + " |"
    for module in sorted(modules):
        paths = sorted(modules[module])
        lines.extend(
            [
                "<details>",
                f"<summary><strong>{module}</strong> · {len(paths)} files</summary>",
                "",
                header,
                separator,
                "| **Module total** | "
                + " | ".join(
                    percent_cell(module_metrics(paths, lane_map)) for lane_map in lane_maps
                )
                + " |",
            ]
        )
        for path in paths:
            lines.append(
                f"| `{path}` | "
                + " | ".join(percent_cell(lane_map.get(path)) for lane_map in lane_maps)
                + " |"
            )
        lines.extend(["", "</details>", ""])

    lines.append(
        "_Coverage is informational. Compare trends within the same lane because coverage backends can count executable lines differently._"
    )
    lines.append("")
    return "\n".join(lines)


def write_report(args: argparse.Namespace) -> None:
    summaries = load_summaries(Path(args.input_dir), args.expected_count)
    markdown = render_markdown(summaries)
    if args.markdown_output:
        with Path(args.markdown_output).open("a", encoding="utf-8") as stream:
            stream.write(markdown)
    print(markdown)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    summarize = subparsers.add_parser("summarize", help="normalize one coverage report")
    summarize.add_argument("--input", required=True)
    summarize.add_argument("--format", choices=("lcov", "cobertura"), required=True)
    summarize.add_argument("--source-root", required=True)
    summarize.add_argument("--markers-dir", required=True)
    summarize.add_argument("--platform", required=True)
    summarize.add_argument("--compiler", required=True)
    summarize.add_argument("--backend", required=True)
    summarize.add_argument("--output", required=True)
    summarize.set_defaults(func=write_lane_summary)

    report = subparsers.add_parser("report", help="write the aggregate job summary")
    report.add_argument("--input-dir", required=True)
    report.add_argument("--expected-count", type=int, default=5)
    report.add_argument("--markdown-output")
    report.set_defaults(func=write_report)
    return parser


def main() -> int:
    args = build_parser().parse_args()
    try:
        args.func(args)
    except (FileNotFoundError, KeyError, ValueError, ET.ParseError, RuntimeError) as error:
        print(f"coverage summary error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
