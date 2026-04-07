from __future__ import annotations

import numpy as np


class MSELoss:
    def __init__(self) -> None:
        self._diff: np.ndarray | None = None

    def forward(self, y_pred: np.ndarray, y_true: np.ndarray) -> float:
        self._diff = y_pred - y_true
        return float(np.mean(self._diff**2))

    def backward(self) -> np.ndarray:
        if self._diff is None:
            raise RuntimeError("MSELoss.backward called before forward")
        return (2.0 / self._diff.size) * self._diff
