from __future__ import annotations

from dataclasses import dataclass, field
from typing import List

import numpy as np

from config import Config
from data import batch_iterator
from loss import MSELoss
from model import MLP


@dataclass
class TrainHistory:
    train_losses: List[float] = field(default_factory=list)
    test_losses: List[float] = field(default_factory=list)
    grad_norms: List[float] = field(default_factory=list)


def train(
    model: MLP,
    loss_fn: MSELoss,
    x_train: np.ndarray,
    y_train: np.ndarray,
    x_test: np.ndarray,
    y_test: np.ndarray,
    cfg: Config,
    rng: np.random.Generator,
) -> TrainHistory:
    if cfg.accumulation_steps <= 0:
        raise ValueError("accumulation_steps must be > 0")

    history = TrainHistory()

    for epoch in range(1, cfg.num_epochs + 1):
        model.zero_grad()
        pending_steps = 0
        epoch_loss_sum = 0.0
        epoch_samples = 0

        for xb, yb, is_last in batch_iterator(
            x_train, y_train, cfg.batch_size, True, rng
        ):
            y_pred = model.forward(xb)
            loss_value = loss_fn.forward(y_pred, yb)
            grad_pred = loss_fn.backward()
            model.backward(grad_pred)

            batch_size = xb.shape[0]
            epoch_loss_sum += loss_value * batch_size
            epoch_samples += batch_size
            pending_steps += 1

            should_update = pending_steps == cfg.accumulation_steps or is_last
            if should_update:
                model.scale_gradients(1.0 / pending_steps)
                model.add_weight_decay(cfg.weight_decay)
                _, clipped_norm = model.clip_grad_norm(cfg.clip_norm)
                history.grad_norms.append(clipped_norm)
                model.step(cfg.learning_rate)
                model.zero_grad()
                pending_steps = 0

        train_loss = epoch_loss_sum / max(epoch_samples, 1)
        test_pred = model.forward(x_test)
        test_loss = loss_fn.forward(test_pred, y_test)

        history.train_losses.append(train_loss)
        history.test_losses.append(test_loss)

        if cfg.log_every > 0 and (
            epoch == 1 or epoch % cfg.log_every == 0 or epoch == cfg.num_epochs
        ):
            print(
                f"Epoch {epoch:03d}/{cfg.num_epochs} | "
                f"train_loss={train_loss:.6f} | test_loss={test_loss:.6f}"
            )

    return history
