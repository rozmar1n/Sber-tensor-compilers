import numpy as np

from config import Config
from data import generate_dataset, train_test_split
from loss import MSELoss
from model import MLP
from trainer import train


def test_training_reduces_loss() -> None:
    cfg = Config(
        hidden_dims=(32, 32),
        learning_rate=0.01,
        batch_size=64,
        num_epochs=60,
        weight_decay=1e-4,
        clip_norm=5.0,
        accumulation_steps=2,
        train_size=1024,
        test_size=256,
        seed=123,
        log_every=0,
    )

    rng = np.random.default_rng(cfg.seed)
    x, y = generate_dataset(cfg.train_size + cfg.test_size, cfg.input_dim, rng)
    x_train, y_train, x_test, y_test = train_test_split(
        x, y, cfg.train_size, cfg.test_size, shuffle=True, rng=rng
    )

    model = MLP(cfg.input_dim, cfg.hidden_dims, cfg.output_dim, rng)
    history = train(model, MSELoss(), x_train, y_train, x_test, y_test, cfg, rng)

    assert history.train_losses[-1] < history.train_losses[0]
    assert history.test_losses[-1] < history.test_losses[0]
    assert history.test_losses[-1] < 0.5
    assert history.test_losses[-1] < 0.1 * history.test_losses[0]
    assert len(history.grad_norms) > 0
