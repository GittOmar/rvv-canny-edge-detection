# Compilers
HOST_CXX = g++
RV_CXX   = riscv64-linux-gnu-g++

# Directories
SRC_DIR        = src
BUILD_HOST_DIR = build_host
BUILD_RV_DIR   = build_rv

# Source files (scalar)
SRCS = $(SRC_DIR)/image_io.cpp \
       $(SRC_DIR)/test_image_gen.cpp \
       $(SRC_DIR)/gaussian.cpp \
       $(SRC_DIR)/sobel.cpp \
       $(SRC_DIR)/magnitude.cpp \
       $(SRC_DIR)/direction.cpp

# RVV source files
RVV_SRCS = $(SRC_DIR)/gaussian_rvv.cpp \
           $(SRC_DIR)/sobel_rvv.cpp

# GoogleTest
GTEST_DIR   = $(HOME)/gtest
GTEST_FLAGS = -I$(GTEST_DIR)/include -L$(GTEST_DIR)/lib -lgtest -lgtest_main -lpthread

# Flags
RV_FLAGS = -std=c++17 -march=rv64gcv -static



#-----------------------------------------------------------------------------------------------------------------------------------------

# ── Scalar builds ─────────────────────────────────────────
$(BUILD_RV_DIR)/canny_O0: $(SRCS) $(SRC_DIR)/main.cpp
	$(RV_CXX) $(RV_FLAGS) -O0 -I$(SRC_DIR) $^ -o $@ -lm

$(BUILD_RV_DIR)/canny_O1: $(SRCS) $(SRC_DIR)/main.cpp
	$(RV_CXX) $(RV_FLAGS) -O1 -I$(SRC_DIR) $^ -o $@ -lm

$(BUILD_RV_DIR)/canny_O2: $(SRCS) $(SRC_DIR)/main.cpp
	$(RV_CXX) $(RV_FLAGS) -O2 -I$(SRC_DIR) $^ -o $@ -lm

$(BUILD_RV_DIR)/canny_O3: $(SRCS) $(SRC_DIR)/main.cpp
	$(RV_CXX) $(RV_FLAGS) -O3 -ftree-vectorize -I$(SRC_DIR) $^ -o $@ -lm

# ── RVV builds ────────────────────────────────────────────
$(BUILD_RV_DIR)/canny_rvv128: $(SRCS) $(RVV_SRCS) $(SRC_DIR)/main_rvv.cpp
	$(RV_CXX) $(RV_FLAGS) -O2 -I$(SRC_DIR) $^ -o $@ -lm

$(BUILD_RV_DIR)/canny_rvv256: $(SRCS) $(RVV_SRCS) $(SRC_DIR)/main_rvv.cpp
	$(RV_CXX) $(RV_FLAGS) -O2 -I$(SRC_DIR) $^ -o $@ -lm

$(BUILD_RV_DIR)/canny_rvv512: $(SRCS) $(RVV_SRCS) $(SRC_DIR)/main_rvv.cpp
	$(RV_CXX) $(RV_FLAGS) -O2 -I$(SRC_DIR) $^ -o $@ -lm

canny_rv: $(BUILD_RV_DIR)/canny_O2

all_opt: $(BUILD_RV_DIR)/canny_O0 $(BUILD_RV_DIR)/canny_O1 $(BUILD_RV_DIR)/canny_O2 $(BUILD_RV_DIR)/canny_O3

all_rvv: $(BUILD_RV_DIR)/canny_rvv128 $(BUILD_RV_DIR)/canny_rvv256 $(BUILD_RV_DIR)/canny_rvv512
#-----------------------------------------------------------------------------------------------------------------------------------------







#-----------------------------------------------------------------------------------------------------------------------------------------
# ── Host test ─────────────────────────────────────────────
HOST_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_HOST_DIR)/%.o,$(SRCS))

$(BUILD_HOST_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(HOST_CXX) -std=c++17 -O2 -I$(SRC_DIR) -c $< -o $@

test: $(HOST_OBJS)
	$(HOST_CXX) -std=c++17 -O2 -I$(SRC_DIR) tests/test_pipeline.cpp $(HOST_OBJS) $(GTEST_FLAGS) -o build_host/test_pipeline
	./build_host/test_pipeline
#-----------------------------------------------------------------------------------------------------------------------------------------







#-----------------------------------------------------------------------------------------------------------------------------------------
# ── Execution & Run Targets ───────────────────────────────────────

run_O0: $(BUILD_RV_DIR)/canny_O0
	@echo "🏃 Executing Scalar Layer: -O0 (No Optimization)..."
	qemu-riscv64 -cpu max,vlen=128 $(BUILD_RV_DIR)/canny_O0 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_O0.png
	@cp /tmp/output_O0.png /tmp/output.png

run_O1: $(BUILD_RV_DIR)/canny_O1
	@echo "🏃 Executing Scalar Layer: -O1 (Basic Optimization)..."
	qemu-riscv64 -cpu max,vlen=128 $(BUILD_RV_DIR)/canny_O1 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_O1.png
	@cp /tmp/output_O1.png /tmp/output.png

run_O2: $(BUILD_RV_DIR)/canny_O2
	@echo "🏃 Executing Scalar Layer: -O2 (Standard Optimization)..."
	qemu-riscv64 -cpu max,vlen=128 $(BUILD_RV_DIR)/canny_O2 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_O2.png
	@cp /tmp/output_O2.png /tmp/output.png

run_O3: $(BUILD_RV_DIR)/canny_O3
	@echo "🏃 Executing Scalar Layer: -O3 (Aggressive Auto-Vectorization)..."
	qemu-riscv64 -cpu max,vlen=128 $(BUILD_RV_DIR)/canny_O3 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_O3.png
	@cp /tmp/output_O3.png /tmp/output.png

run_rvv128: $(BUILD_RV_DIR)/canny_rvv128
	@echo "🔥 Executing Handwritten RVV Extension Pipeline [VLEN=128]..."
	qemu-riscv64 -cpu max,vlen=128 $(BUILD_RV_DIR)/canny_rvv128 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_rvv128.png
	@cp /tmp/output_rvv128.png /tmp/output.png

run_rvv256: $(BUILD_RV_DIR)/canny_rvv256
	@echo "🔥 Executing Handwritten RVV Extension Pipeline [VLEN=256]..."
	qemu-riscv64 -cpu max,vlen=256 $(BUILD_RV_DIR)/canny_rvv256 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_rvv256.png
	@cp /tmp/output_rvv256.png /tmp/output.png

run_rvv512: $(BUILD_RV_DIR)/canny_rvv512
	@echo "🔥 Executing Handwritten RVV Extension Pipeline [VLEN=512]..."
	qemu-riscv64 -cpu max,vlen=512 $(BUILD_RV_DIR)/canny_rvv512 /tmp/test_input.raw 128 128 /tmp/test_output.raw
	convert -depth 8 -size 128x128 gray:/tmp/test_input.raw /tmp/input.png
	convert -depth 8 -size 128x128 gray:/tmp/test_output.raw /tmp/output_rvv512.png
	@cp /tmp/output_rvv512.png /tmp/output.png

# ── Automated Benchmarking Suite ──────────────────────────────────
run_all:
	@echo "========================================================="
	@echo "   STARTING EXECUTION SWEEP FOR ALL 7 TOPOLOGIES         "
	@echo "========================================================="
	@if [ ! -f /tmp/test_input.raw ]; then \
		echo "⚠️ /tmp/test_input.raw missing! Generating a default rectangle pattern..."; \
		convert -size 128x128 xc:black -fill white -draw "rectangle 20,40 108,88" -depth 8 gray:/tmp/test_input.raw; \
	fi
	@echo "\n[1/7] Optimization Topology: Scalar -O0"
	@$(MAKE) --no-print-directory run_O0 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n[2/7] Optimization Topology: Scalar -O1"
	@$(MAKE) --no-print-directory run_O1 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n[3/7] Optimization Topology: Scalar -O2"
	@$(MAKE) --no-print-directory run_O2 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n[4/7] Optimization Topology: Scalar -O3 (Compiler Auto-Vectorized)"
	@$(MAKE) --no-print-directory run_O3 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n[5/7] Hardware Topology: RVV Handwritten (VLEN=128)"
	@$(MAKE) --no-print-directory run_rvv128 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n[6/7] Hardware Topology: RVV Handwritten (VLEN=256)"
	@$(MAKE) --no-print-directory run_rvv256 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n[7/7] Hardware Topology: RVV Handwritten (VLEN=512)"
	@$(MAKE) --no-print-directory run_rvv512 | grep -E "Gaussian|Sobel|Magnitude|Direction|Total" || true
	@echo "\n========================================================="
	@echo "✅ Benchmarking Complete! All structural outputs exported."
	@echo "========================================================="
#-----------------------------------------------------------------------------------------------------------------------------------------






#-----------------------------------------------------------------------------------------------------------------------------------------
# ── Clean ─────────────────────────────────────────────────
clean:
	rm -f $(BUILD_HOST_DIR)/*.o $(BUILD_RV_DIR)/canny_*

.PHONY: run clean test all_opt all_rvv canny_rv
#-----------------------------------------------------------------------------------------------------------------------------------------







#-----------------------------------------------------------------------------------------------------------------------------------------

# ── Image Generation & Auto-Run Targets ───────────────────
.PHONY: rect tri sphere random

rect:
	@echo "🟩 Generating Rectangle Input..."
	convert -size 128x128 xc:black -fill white -draw "rectangle 20,40 108,88" -depth 8 gray:/tmp/test_input.raw


tri:
	@echo "🔺 Generating Triangle Input..."
	convert -size 128x128 xc:black -fill white -draw "polygon 64,20 20,108 108,108" -depth 8 gray:/tmp/test_input.raw


sphere:
	@echo "⚪ Generating Sphere Input..."
	convert -size 128x128 radial-gradient:white-black -depth 8 gray:/tmp/test_input.raw

random:
	@echo "🎲 Generating Random Noise Input..."
	dd if=/dev/urandom of=/tmp/test_input.raw bs=1 count=16384



IMG ?= /tmp/my_draw.png
paint:
	@echo "🎨 Processing image from default path: $(IMG)"
	@python3 -c "import numpy as np; from PIL import Image; img = Image.open('$(IMG)').convert('L').resize((128,128)); np.array(img).tofile('/tmp/test_input.raw')"
#-----------------------------------------------------------------------------------------------------------------------------------------