from __future__ import annotations

from typing import List, Tuple

import numpy as np


class Linear:
    def __init__(
        self, in_features: int, out_features: int, rng: np.random.Generator
    ) -> None:
        scale = np.sqrt(2.0 / in_features)
        self.W = rng.normal(0.0, scale, size=(out_features, in_features)).astype(
            np.float64
        )
        self.b = np.zeros(out_features, dtype=np.float64)

        self.dW = np.zeros_like(self.W)
        self.db = np.zeros_like(self.b)
        self._input_cache: np.ndarray | None = None

    def forward(self, x: np.ndarray) -> np.ndarray:
        self._input_cache = x
        return x @ self.W.T + self.b

    def backward(self, grad_output: np.ndarray) -> np.ndarray:
        if self._input_cache is None:
            raise RuntimeError("Linear.backward called before forward")

        self.dW += grad_output.T @ self._input_cache
        self.db += grad_output.sum(axis=0)
        grad_input = grad_output @ self.W
        return grad_input

    def zero_grad(self) -> None:
        self.dW.fill(0.0)
        self.db.fill(0.0)

    def parameters_and_grads(self) -> List[Tuple[np.ndarray, np.ndarray]]:
        return [(self.W, self.dW), (self.b, self.db)]


class ReLU:
    def __init__(self) -> None:
        self._mask: np.ndarray | None = None

    def forward(self, x: np.ndarray) -> np.ndarray:
        self._mask = x > 0.0
        return np.where(self._mask, x, 0.0)

    def backward(self, grad_output: np.ndarray) -> np.ndarray:
        if self._mask is None:
            raise RuntimeError("ReLU.backward called before forward")
        return grad_output * self._mask
