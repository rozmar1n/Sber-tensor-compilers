from __future__ import annotations

from typing import Iterator, Tuple

import numpy as np


def target_function(x: np.ndarray) -> np.ndarray:
    x = np.asarray(x, dtype=np.float64)
    squeeze = x.ndim == 1
    if squeeze:
        x = x[None, :]

    if x.shape[1] != 4:
        raise ValueError(f"Expected input dimension 4, got {x.shape[1]}")

    y = np.empty((x.shape[0], 4), dtype=np.float64)
    y[:, 0] = x[:, 0] ** 2
    y[:, 1] = 3.0 * x[:, 1]
    y[:, 2] = 5.0 * x[:, 3] - x[:, 2]
    y[:, 3] = 3.0
    return y[0] if squeeze else y


def generate_dataset(
    num_samples: int,
    input_dim: int,
    rng: np.random.Generator,
) -> Tuple[np.ndarray, np.ndarray]:
    if input_dim != 4:
        raise ValueError(
            "Target function in this assignment is defined for input_dim=4"
        )

    x = rng.normal(loc=0.0, scale=1.0, size=(
        num_samples, input_dim)).astype(np.float64)
    y = target_function(x)
    return x, y


def train_test_split(
    x: np.ndarray,
    y: np.ndarray,
    train_size: int,
    test_size: int,
    shuffle: bool,
    rng: np.random.Generator,
) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    n = x.shape[0]
    if train_size + test_size > n:
        raise ValueError("train_size + test_size cannot exceed dataset size")

    indices = np.arange(n)
    if shuffle:
        rng.shuffle(indices)
    indices = indices[: train_size + test_size]

    train_idx = indices[:train_size]
    test_idx = indices[train_size: train_size + test_size]
    return x[train_idx], y[train_idx], x[test_idx], y[test_idx]


def batch_iterator(
    x: np.ndarray,
    y: np.ndarray,
    batch_size: int,
    shuffle: bool,
    rng: np.random.Generator,
) -> Iterator[tuple[np.ndarray, np.ndarray, bool]]:
    n = x.shape[0]
    if batch_size <= 0:
        raise ValueError("batch_size must be > 0")

    indices = np.arange(n)
    if shuffle:
        rng.shuffle(indices)

    for start in range(0, n, batch_size):
        end = min(start + batch_size, n)
        batch_idx = indices[start:end]
        is_last = end >= n
        yield x[batch_idx], y[batch_idx], is_last
