# KERNELS

Мини-проект с kernel-level реализациями MatMul и Conv2D для курса по тензорным компиляторам.

## Реализовано

- `matmul_naive`
- `matmul_cache_friendly` через транспонирование `B`
- `matmul_tiled`
- `matmul_vectorized` с AVX2 при `-DKERNELS_ENABLE_AVX2=ON`, иначе fallback
- `conv2d_naive` для NCHW valid convolution
- `im2col`
- `conv2d_im2col` через Im2Col + MatMul
- GoogleTest correctness tests
- Google Benchmark
- CSV results и PNG-графики

## Сборка

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

AVX2:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DKERNELS_ENABLE_AVX2=ON
cmake --build build
```

Требуются установленные GoogleTest и Google Benchmark.

## Тесты

```bash
ctest --test-dir build --output-on-failure
```

CTest запускает отдельные GoogleTest cases.

## Benchmark

Обычный запуск:

```bash
./build/kernels_bench
```

CSV для Google Benchmark:

```bash
./build/kernels_bench \
  --benchmark_out=results/benchmark_results.csv \
  --benchmark_out_format=csv
```

## Графики

В репозитории уже сохранены графики:

- `results/matmul_benchmark.png`
- `results/conv_benchmark.png`

Построить графики из Google Benchmark CSV:

```bash
python3 scripts/plot_results.py results/benchmark_results.csv results
```

## Ограничения

- Только `float32`.
- Матрицы: contiguous row-major.
- Тензоры: NCHW.
- Conv2D только valid convolution: stride и padding не реализованы.
- AVX2 включается явно через CMake option.
