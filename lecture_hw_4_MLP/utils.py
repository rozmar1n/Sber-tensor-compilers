from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

from model import MLP


def make_rng(seed: int) -> np.random.Generator:
    np.random.seed(seed)
    return np.random.default_rng(seed)


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def save_loss_curve(
    train_losses: list[float], test_losses: list[float], out_path: Path
) -> None:
    plt.figure(figsize=(8, 5))
    plt.plot(train_losses, label="train")
    plt.plot(test_losses, label="test")
    plt.xlabel("epoch")
    plt.ylabel("MSE")
    plt.title("MLP Loss Curve")
    plt.grid(alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path)
    plt.close()


def save_sample_predictions(
    model: MLP,
    x_test: np.ndarray,
    y_test: np.ndarray,
    out_path: Path,
    n_samples: int = 8,
) -> None:
    n = min(n_samples, x_test.shape[0])
    y_pred = model.forward(x_test[:n])

    with out_path.open("w", encoding="utf-8") as f:
        f.write("Sample predictions (y_pred vs y_true)\n")
        f.write("=" * 70 + "\n")
        for i in range(n):
            pred = np.array2string(
                y_pred[i], precision=4, suppress_small=False)
            true = np.array2string(
                y_test[i], precision=4, suppress_small=False)
            f.write(f"{i:02d} | pred={pred} | true={true}\n")
