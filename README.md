# Canny Edge Detection on RISC-V Vector Extension (RVV)

A high-performance implementation of a Canny-style Edge Detection pipeline optimized for the **RISC-V Vector Extension (RVV 1.0)**.

## Team Members

* Omar Ibrahim
* Omar Foad
* Omar Khalid
* Omar Mansour
* Hazem Abeed
* Mohamed Amer

---

## Project Overview

This project implements an image-processing pipeline on the **RV64GCV** architecture using the **RISC-V Vector Extension (RVV)**. The objective is to investigate vector-length-agnostic programming, compiler optimizations, and RVV intrinsics while building and optimizing a complete edge detection workflow.

The implementation begins with a scalar baseline and progresses through profiling, auto-vectorization, and hand-optimized RVV kernels. Correctness is verified across multiple vector lengths (VLEN = 128, 256, and 512) using QEMU.

---

## Pipeline Stages

1. Image Input / Output
2. Gaussian Blur (5×5 Convolution)
3. Sobel Gradient Computation (Gx, Gy)
4. Gradient Magnitude Calculation
5. Gradient Direction Quantization
6. Performance Profiling
7. RVV Intrinsic Optimization

---

## Project Structure

```text
main
│
├── feature/image-io
│   ├── image_io
│   └── test_image_gen
│
├── feature/scalar-pipeline
│   ├── gaussian
│   ├── sobel
│   ├── magnitude
│   ├── direction
│   └── main
│
├── feature/host-tests
│   ├── GoogleTest setup
│   └── pipeline tests
│
└── feature/rvv-kernels
    ├── gaussian_rvv
    ├── sobel_rvv
    └── main_rvv
```

---

## Processing Pipeline

```text
Input Image
      │
      ▼
Gaussian Blur
      │
      ▼
Sobel Gx / Gy
      │
      ▼
Gradient Magnitude
      │
      ▼
Gradient Direction
      │
      ▼
Output Edge Map
```

---

## Technologies Used

* C++
* RISC-V RV64GCV
* RVV 1.0 Intrinsics
* QEMU
* GoogleTest
* GNU Make
* GCC RISC-V Toolchain

---

## Input and Output

### Input Image

![Input Image](images/input.png)

### Output Image

![Output Image](images/output.png)

---

## Build Instructions

### Run Host Tests

```bash
make test
```

### Build RISC-V Binary

```bash
make canny_rv
```

### Execute on QEMU

```bash
make run
```

---

## Validation

The project is validated using:

* Uniform image tests
* Impulse response tests
* Vertical edge detection tests
* Horizontal edge detection tests
* Diagonal edge detection tests
* Scalar vs RVV equivalence testing
* VLEN 128, 256, and 512 verification

---

## Learning Outcomes

* RISC-V Vector Programming
* SIMD Optimization Techniques
* Compiler Auto-Vectorization
* Performance Profiling
* Vector-Length Agnostic Design
* RVV Intrinsic Development
* Image Processing Algorithms

---

## Authors

Developed by **Omar Ibrahim**, **Omar Foad**, **Omar Khalid**, **Omar Mansour**, **Hazem Abeed**, and **Mohamed Amer** as part of a RISC-V Vector Computing project focused on implementing and optimizing a Canny Edge Detection pipeline using the RISC-V Vector Extension (RVV 1.0).
