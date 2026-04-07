import numpy as np

from loss import MSELoss


def test_mse_forward_backward() -> None:
    loss_fn = MSELoss()
    y_pred = np.array([[2.0, 0.0], [1.0, -1.0]])
    y_true = np.array([[1.0, 1.0], [1.0, 1.0]])

    loss = loss_fn.forward(y_pred, y_true)
    grad = loss_fn.backward()

    expected_loss = np.mean((y_pred - y_true) ** 2)
    expected_grad = 2.0 * (y_pred - y_true) / y_pred.size

    assert abs(loss - expected_loss) < 1e-12
    np.testing.assert_allclose(grad, expected_grad)
