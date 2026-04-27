#!/usr/bin/env python3

"""Plot benchmark CSV produced by kernels_bench."""

from __future__ import annotations

import csv
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402


def read_rows(csv_path: Path) -> list[dict[str, str]]:
    with csv_path.open(newline="") as csv_file:
        reader = csv.DictReader(csv_file)
        required = {
            "operation",
            "variant",
            "M",
            "K",
            "N",
            "input_shape",
            "kernel_shape",
            "time_ms",
        }
        if reader.fieldnames is None or set(reader.fieldnames) != required:
            raise ValueError(f"unexpected CSV header in {csv_path}")
        return list(reader)


def plot_matmul(rows: list[dict[str, str]], output_dir: Path) -> None:
    series: dict[str, list[tuple[int, float]]] = defaultdict(list)
    for row in rows:
        if row["operation"] != "matmul":
            continue
        series[row["variant"]].append((int(row["M"]), float(row["time_ms"])))

    if not series:
        raise ValueError("CSV does not contain matmul benchmark rows")

    fig, ax = plt.subplots(figsize=(8, 5))
    for variant in sorted(series):
        points = sorted(series[variant])
        sizes = [size for size, _ in points]
        times = [time_ms for _, time_ms in points]
        ax.plot(sizes, times, marker="o", linewidth=2, label=variant)

    ax.set_title("MatMul benchmark")
    ax.set_xlabel("Matrix size (M = K = N)")
    ax.set_ylabel("Average time, ms")
    ax.set_xticks(sorted({size for points in series.values() for size, _ in points}))
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(output_dir / "matmul_benchmark.png", dpi=160)
    plt.close(fig)


def plot_conv(rows: list[dict[str, str]], output_dir: Path) -> None:
    series: dict[str, list[tuple[str, float]]] = defaultdict(list)
    shape_order: list[str] = []
    for row in rows:
        if row["operation"] != "conv":
            continue
        shape = row["input_shape"]
        if shape not in shape_order:
            shape_order.append(shape)
        series[row["variant"]].append((shape, float(row["time_ms"])))

    if not series:
        raise ValueError("CSV does not contain convolution benchmark rows")

    x_positions = list(range(len(shape_order)))
    fig, ax = plt.subplots(figsize=(8, 5))
    for variant in sorted(series):
        by_shape = dict(series[variant])
        times = [by_shape[shape] for shape in shape_order]
        ax.plot(x_positions, times, marker="o", linewidth=2, label=variant)

    ax.set_title("Conv2D benchmark")
    ax.set_xlabel("Input shape (N x C x H x W)")
    ax.set_ylabel("Average time, ms")
    ax.set_xticks(x_positions)
    ax.set_xticklabels(shape_order, rotation=20, ha="right")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(output_dir / "conv_benchmark.png", dpi=160)
    plt.close(fig)


def main(argv: list[str]) -> int:
    if len(argv) != 3:
        print("Usage: plot_results.py <benchmark_results.csv> <output_dir>", file=sys.stderr)
        return 2

    csv_path = Path(argv[1])
    output_dir = Path(argv[2])
    output_dir.mkdir(parents=True, exist_ok=True)

    rows = read_rows(csv_path)
    plot_matmul(rows, output_dir)
    plot_conv(rows, output_dir)

    print(output_dir / "matmul_benchmark.png")
    print(output_dir / "conv_benchmark.png")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
