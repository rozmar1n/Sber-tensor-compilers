import numpy as np

from model import MLP


def test_mlp_forward_backward() -> None:
    rng = np.random.default_rng(0)
    model = MLP(input_dim=4, hidden_dims=(8, 8), output_dim=4, rng=rng)
    x = rng.normal(size=(7, 4))

    y = model.forward(x)
    assert y.shape == (7, 4)

    grad_output = np.ones_like(y)
    grad_input = model.backward(grad_output)
    assert grad_input.shape == (7, 4)

    grad_norm = model.global_grad_norm()
    assert grad_norm > 0.0

    model.clip_grad_norm(0.5)
    assert model.global_grad_norm() <= 0.5000001
