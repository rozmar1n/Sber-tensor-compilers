#!/usr/bin/env python3

"""Plot Google Benchmark CSV produced by kernels_bench."""

from __future__ import annotations

import csv
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402


def read_csv_rows(csv_path: Path) -> list[dict[str, str]]:
    with csv_path.open(newline="") as csv_file:
        lines = csv_file.readlines()

    header_index = next(
        (index for index, line in enumerate(lines) if line.startswith("name,")),
        None,
    )
    if header_index is None:
        header_index = next(
            (index for index, line in enumerate(lines) if line.startswith("operation,")),
            None,
        )
    if header_index is None:
        raise ValueError(f"could not find CSV header in {csv_path}")

    reader = csv.DictReader(lines[header_index:])
    if reader.fieldnames is None:
        raise ValueError(f"unexpected CSV header in {csv_path}")
    return list(reader)


def time_to_ms(value: str, unit: str) -> float:
    time = float(value)
    if unit == "ns":
        return time / 1_000_000.0
    if unit == "us":
        return time / 1_000.0
    if unit == "ms":
        return time
    if unit == "s":
        return time * 1_000.0
    raise ValueError(f"unsupported time unit: {unit}")


def parse_benchmark_name(name: str) -> tuple[str, list[int]] | None:
    parts = name.strip('"').split("/")
    benchmark_name = parts[0]
    args: list[int] = []

    for part in parts[1:]:
        value = part.rsplit(":", maxsplit=1)[-1]
        try:
            args.append(int(value))
        except ValueError:
            return None

    return benchmark_name, args


def normalize_google_benchmark_rows(rows: list[dict[str, str]]) -> list[dict[str, str]]:
    normalized: list[dict[str, str]] = []
    for row in rows:
        name = row["name"].strip('"')
        parsed_name = parse_benchmark_name(name)
        if parsed_name is None:
            continue

        benchmark_name, args = parsed_name
        time_ms = time_to_ms(row["real_time"], row["time_unit"])

        if benchmark_name.startswith("bench_matmul_"):
            if not args:
                raise ValueError(f"matmul benchmark has no size: {name}")
            variant = benchmark_name.removeprefix("bench_matmul_")
            size = str(args[0])
            normalized.append(
                {
                    "operation": "matmul",
                    "variant": variant,
                    "M": size,
                    "K": size,
                    "N": size,
                    "input_shape": "",
                    "kernel_shape": "",
                    "time_ms": str(time_ms),
                }
            )
            continue

        if benchmark_name.startswith("bench_conv2d_"):
            if len(args) != 7:
                raise ValueError(f"conv benchmark must have 7 args: {name}")
            batch, channels_in, height, width, channels_out, kernel_h, kernel_w = args
            variant = benchmark_name.removeprefix("bench_conv2d_")
            normalized.append(
                {
                    "operation": "conv",
                    "variant": variant,
                    "M": "",
                    "K": "",
                    "N": "",
                    "input_shape": f"{batch}x{channels_in}x{height}x{width}",
                    "kernel_shape": f"{channels_out}x{channels_in}x{kernel_h}x{kernel_w}",
                    "time_ms": str(time_ms),
                }
            )
            continue

    return normalized


def read_rows(csv_path: Path) -> list[dict[str, str]]:
    rows = read_csv_rows(csv_path)
    fieldnames = set(rows[0].keys()) if rows else set()
    simple_header = {
        "operation",
        "variant",
        "M",
        "K",
        "N",
        "input_shape",
        "kernel_shape",
        "time_ms",
    }
    google_header = {
        "name",
        "iterations",
        "real_time",
        "cpu_time",
        "time_unit",
        "bytes_per_second",
        "items_per_second",
        "label",
        "error_occurred",
        "error_message",
    }

    if simple_header.issubset(fieldnames):
        return rows
    if google_header.issubset(fieldnames):
        return normalize_google_benchmark_rows(rows)
    raise ValueError(f"unexpected CSV header in {csv_path}")


def display_variant(variant: str) -> str:
    if variant == "im2col":
        return variant
    return variant.replace("_", " ")


def variant_order(variant: str) -> tuple[int, str]:
    order = {
        "naive": 0,
        "cache_friendly": 1,
        "tiled": 2,
        "vectorized": 3,
        "im2col": 1,
    }
    return order.get(variant, 99), variant


def plot_matmul(rows: list[dict[str, str]], output_dir: Path) -> Path | None:
    series: dict[str, list[tuple[int, float]]] = defaultdict(list)
    for row in rows:
        if row["operation"] != "matmul":
            continue
        series[row["variant"]].append((int(row["M"]), float(row["time_ms"])))

    if not series:
        return None

    fig, ax = plt.subplots(figsize=(8, 5))
    for variant in sorted(series, key=variant_order):
        points = sorted(series[variant])
        sizes = [size for size, _ in points]
        times = [time_ms for _, time_ms in points]
        ax.plot(sizes, times, marker="o", linewidth=2, label=display_variant(variant))

    ax.set_title("MatMul benchmark")
    ax.set_xlabel("Matrix size (M = K = N)")
    ax.set_ylabel("Average time, ms")
    ax.set_xticks(sorted({size for points in series.values() for size, _ in points}))
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    output_path = output_dir / "matmul_benchmark.png"
    fig.savefig(output_path, dpi=160)
    plt.close(fig)
    return output_path


def plot_conv(rows: list[dict[str, str]], output_dir: Path) -> Path | None:
    series: dict[str, list[tuple[str, float]]] = defaultdict(list)
    shape_order: list[str] = []
    for row in rows:
        if row["operation"] != "conv":
            continue
        shape = f'{row["input_shape"]}, kernel={row["kernel_shape"]}'
        if shape not in shape_order:
            shape_order.append(shape)
        series[row["variant"]].append((shape, float(row["time_ms"])))

    if not series:
        return None

    x_positions = list(range(len(shape_order)))
    fig, ax = plt.subplots(figsize=(8, 5))
    for variant in sorted(series, key=variant_order):
        by_shape = dict(series[variant])
        times = [by_shape[shape] for shape in shape_order]
        ax.plot(x_positions, times, marker="o", linewidth=2, label=display_variant(variant))

    ax.set_title("Conv2D benchmark")
    ax.set_xlabel("Input shape and kernel shape")
    ax.set_ylabel("Average time, ms")
    ax.set_xticks(x_positions)
    ax.set_xticklabels(shape_order, rotation=20, ha="right")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    output_path = output_dir / "conv_benchmark.png"
    fig.savefig(output_path, dpi=160)
    plt.close(fig)
    return output_path


def main(argv: list[str]) -> int:
    if len(argv) != 3:
        print("Usage: plot_results.py <benchmark_results.csv> <output_dir>", file=sys.stderr)
        return 2

    csv_path = Path(argv[1])
    output_dir = Path(argv[2])
    output_dir.mkdir(parents=True, exist_ok=True)

    rows = read_rows(csv_path)
    output_paths = [
        path
        for path in (plot_matmul(rows, output_dir), plot_conv(rows, output_dir))
        if path is not None
    ]

    if not output_paths:
        raise ValueError("CSV does not contain supported benchmark rows")

    for output_path in output_paths:
        print(output_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
