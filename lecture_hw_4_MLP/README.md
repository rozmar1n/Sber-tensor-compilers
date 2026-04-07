# MLP From Scratch (NumPy)

Реализация многослойного перцептрона для задания по теме MLP.

## Что реализовано

- Линейные слои `y = Wx + b` с ручным `forward/backward`.
- Активация `ReLU` с ручным `forward/backward`.
- Функция потерь `MSELoss` с ручным вычислением градиента.
- Модель `MLP` с произвольным числом скрытых слоёв.
- Цикл обучения по эпохам и батчам.
- `L2`-регуляризация (`weight_decay`).
- Gradient clipping по глобальной норме.
- Gradient accumulation (`accumulation_steps`).
- Генерация датасета по целевой функции:
  `f(x1, x2, x3, x4) = (x1^2, 3x2, 5x4 - x3, 3)^T`.
- Сохранение артефактов обучения:
  - `results/loss_curve.png`
  - `results/sample_predictions.txt`

## Структура

```text
lecture_hw_4_MLP/
├── MLP.pdf
├── mlp_tz.md
├── README.md
├── requirements.txt
├── main.py
├── config.py
├── data.py
├── layers.py
├── loss.py
├── model.py
├── trainer.py
├── utils.py
└── tests/
    ├── conftest.py
    ├── test_layers.py
    ├── test_loss.py
    ├── test_model.py
    └── test_training.py
```

## Запуск

```bash
cd lecture_hw_4_MLP
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python main.py
```

## Тесты

```bash
cd lecture_hw_4_MLP
pytest -q
```
