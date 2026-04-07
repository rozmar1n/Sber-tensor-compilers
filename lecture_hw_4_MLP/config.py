from dataclasses import dataclass
from typing import Tuple


@dataclass(slots=True)
class Config:
    input_dim: int = 4
    hidden_dims: Tuple[int, ...] = (64, 64)
    output_dim: int = 4

    learning_rate: float = 0.01
    batch_size: int = 64
    num_epochs: int = 200

    weight_decay: float = 1e-4
    clip_norm: float = 1.0
    accumulation_steps: int = 1

    train_size: int = 8000
    test_size: int = 2000
    seed: int = 42
    log_every: int = 20


def default_config() -> Config:
    return Config()
