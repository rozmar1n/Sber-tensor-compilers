import numpy as np

from layers import Linear, ReLU


def test_linear_forward_backward_shapes() -> None:
    rng = np.random.default_rng(0)
    layer = Linear(in_features=4, out_features=3, rng=rng)
    x = rng.normal(size=(5, 4))

    out = layer.forward(x)
    assert out.shape == (5, 3)

    grad_out = np.ones_like(out)
    grad_in = layer.backward(grad_out)

    assert grad_in.shape == (5, 4)
    assert layer.dW.shape == (3, 4)
    assert layer.db.shape == (3,)
    assert np.all(np.isfinite(layer.dW))
    assert np.all(np.isfinite(layer.db))


def test_relu_backward_mask() -> None:
    relu = ReLU()
    x = np.array([[-1.0, 0.5, 2.0]])
    out = relu.forward(x)
    grad = relu.backward(np.ones_like(out))

    np.testing.assert_allclose(out, np.array([[0.0, 0.5, 2.0]]))
    np.testing.assert_allclose(grad, np.array([[0.0, 1.0, 1.0]]))
