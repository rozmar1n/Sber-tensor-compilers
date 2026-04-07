from __future__ import annotations

from typing import Iterator, List, Tuple

import numpy as np

from layers import Linear, ReLU


LayerType = Linear | ReLU


class MLP:
    def __init__(
        self,
        input_dim: int,
        hidden_dims: tuple[int, ...],
        output_dim: int,
        rng: np.random.Generator,
    ) -> None:
        dims = [input_dim, *hidden_dims, output_dim]
        self.layers: List[LayerType] = []
        self.linear_layers: List[Linear] = []

        for i in range(len(dims) - 1):
            linear = Linear(dims[i], dims[i + 1], rng)
            self.layers.append(linear)
            self.linear_layers.append(linear)
            if i < len(dims) - 2:
                self.layers.append(ReLU())

    def forward(self, x: np.ndarray) -> np.ndarray:
        out = x
        for layer in self.layers:
            out = layer.forward(out)
        return out

    def backward(self, grad_output: np.ndarray) -> np.ndarray:
        grad = grad_output
        for layer in reversed(self.layers):
            grad = layer.backward(grad)
        return grad

    def zero_grad(self) -> None:
        for layer in self.linear_layers:
            layer.zero_grad()

    def parameters_and_grads(self) -> Iterator[Tuple[np.ndarray, np.ndarray]]:
        for layer in self.linear_layers:
            for param_grad in layer.parameters_and_grads():
                yield param_grad

    def scale_gradients(self, scale: float) -> None:
        for _, grad in self.parameters_and_grads():
            grad *= scale

    def add_weight_decay(self, weight_decay: float, include_bias: bool = False) -> None:
        if weight_decay <= 0.0:
            return
        for layer in self.linear_layers:
            layer.dW += weight_decay * layer.W
            if include_bias:
                layer.db += weight_decay * layer.b

    def global_grad_norm(self) -> float:
        total = 0.0
        for _, grad in self.parameters_and_grads():
            total += float(np.sum(grad * grad))
        return float(np.sqrt(total))

    def clip_grad_norm(self, max_norm: float) -> tuple[float, float]:
        before = self.global_grad_norm()
        if max_norm <= 0.0 or before <= max_norm:
            return before, before

        scale = max_norm / (before + 1e-12)
        self.scale_gradients(scale)
        after = self.global_grad_norm()
        return before, after

    def step(self, learning_rate: float) -> None:
        for param, grad in self.parameters_and_grads():
            param -= learning_rate * grad
