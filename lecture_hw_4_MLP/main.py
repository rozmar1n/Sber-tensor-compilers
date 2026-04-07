from __future__ import annotations

from pathlib import Path

from config import default_config
from data import generate_dataset, train_test_split
from loss import MSELoss
from model import MLP
from trainer import train
from utils import ensure_dir, make_rng, save_loss_curve, save_sample_predictions


def main() -> None:
    cfg = default_config()
    rng = make_rng(cfg.seed)

    x, y = generate_dataset(cfg.train_size + cfg.test_size, cfg.input_dim, rng)
    x_train, y_train, x_test, y_test = train_test_split(
        x=x,
        y=y,
        train_size=cfg.train_size,
        test_size=cfg.test_size,
        shuffle=True,
        rng=rng,
    )

    model = MLP(
        input_dim=cfg.input_dim,
        hidden_dims=cfg.hidden_dims,
        output_dim=cfg.output_dim,
        rng=rng,
    )
    loss_fn = MSELoss()

    history = train(model, loss_fn, x_train, y_train, x_test, y_test, cfg, rng)

    results_dir = Path(__file__).resolve().parent / "results"
    ensure_dir(results_dir)
    save_loss_curve(
        history.train_losses, history.test_losses, results_dir / "loss_curve.png"
    )
    save_sample_predictions(
        model, x_test, y_test, results_dir / "sample_predictions.txt"
    )

    print("\nTraining completed.")
    print(f"Final train loss: {history.train_losses[-1]:.6f}")
    print(f"Final test loss:  {history.test_losses[-1]:.6f}")
    print(f"Saved artifacts to: {results_dir}")


if __name__ == "__main__":
    main()
