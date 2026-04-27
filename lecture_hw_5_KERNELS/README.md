# KERNELS

Мини-проект с kernel-level реализациями MatMul и Conv2D для курса по тензорным компиляторам.

## Реализовано

- `matmul_naive`
- `matmul_cache_friendly` через транспонирование `B`
- `matmul_tiled`
- `matmul_vectorized` с AVX2 при `-DKERNELS_ENABLE_AVX2=ON`, иначе fallback на `matmul_cache_friendly`
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

Только библиотека без тестов и benchmark:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DKERNELS_BUILD_TESTS=OFF \
  -DKERNELS_BUILD_BENCHMARKS=OFF
cmake --build build
```

AVX2:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DKERNELS_ENABLE_AVX2=ON
cmake --build build
```

Требуются установленные GoogleTest и Google Benchmark.
Если `KERNELS_BUILD_TESTS=OFF` или `KERNELS_BUILD_BENCHMARKS=OFF`, соответствующая зависимость не нужна.

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

Методика:

- benchmark-данные создаются Google Benchmark, подготовка входных данных вынесена за измеряемый цикл;
- `conv2d_im2col` включает Im2Col, reshape kernel, MatMul и обратное преобразование layout;
- текущие reference results получены на Intel Core i5-1235U, GCC 13.3.0, `Release`, `KERNELS_ENABLE_AVX2=ON`;
- установленная Google Benchmark library сообщает `Library was built as DEBUG`, поэтому абсолютные времена могут быть шумными, однако сравнение вариантов внутри одного запуска остается полезным.

Краткие результаты из `results/benchmark_results.csv`, время в ms:

| Kernel | Case | Time |
| --- | --- | ---: |
| `matmul_naive` | 512x512x512 | 91.739 |
| `matmul_cache_friendly` | 512x512x512 | 62.098 |
| `matmul_tiled` | 512x512x512, tile=32 | 52.641 |
| `matmul_vectorized` | 512x512x512 | 8.026 |
| `conv2d_naive` | 1x16x64x64, kernel=32x16x3x3 | 11.842 |
| `conv2d_im2col` | 1x16x64x64, kernel=32x16x3x3 | 1.470 |

Главный эффект: транспонирование `B` улучшает locality, tiling дополнительно снижает промахи cache, а AVX2 дает самый заметный прирост для MatMul. Im2Col быстрее naive Conv2D на больших канальных случаях, потому что основная работа сводится к оптимизированному MatMul.

## Графики

В репозитории уже сохранены графики:

- `results/matmul_benchmark.png`
- `results/conv_benchmark.png`

Построить графики из Google Benchmark CSV:

```bash
python3 scripts/plot_results.py results/benchmark_results.csv results
```

## Ограничения

- Только C++ `float` / IEEE single precision.
- Матрицы: contiguous row-major, `A[i*K+k]`, `B[k*N+j]`, `C[i*N+j]`.
- Тензоры: NCHW, `input[((n*C+c)*H+h)*W+w]`.
- Conv2D только valid convolution: stride и padding не реализованы.
- AVX2 включается явно через CMake option, runtime dispatch не реализован.
