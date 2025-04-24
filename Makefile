# ----------------------------------------------------------------------------------------
# File:				Makefile
# Author:       	Isaiah Hendrick
# Project:        	Master's Project - PYNQ Voronoi Acceleration
# Organization:   	Grand Valley State University
# Date:           	April 17, 2025
#
# Description:		Makefile for Voronoi project - Sets up virtual environment to run
#					Python scripts and compiles C code before running all files and
#					logging system info and timing comparisons. Note: Running 
# 					"voronoi_omp.c" includes a setting at runtime which configures the 
#					number of threads being used, configure this below to match the 
# 					number of processing units your	system has available (or less if 
#					you wish).
# ----------------------------------------------------------------------------------------

#####################################################################################################
# IMPORTANT: Edit these values manually to match output from PYNQ notebook, used for logs and graphing
PYNQ_SIZE := 4096
PYNQ_TIME := 0.605872
PYNQ_SEQ_C_TIME := 6.573427
PYNQ_OMP_C_TIME := 3.583120
#####################################################################################################

# Project directories
SRC_DIR := src
C_SRC_DIR := $(SRC_DIR)/c
PY_SRC_DIR := $(SRC_DIR)/python
DEV_DIR := $(SRC_DIR)/dev_tools
BIN_DIR := bin
LOG_DIR := logs
DATA_DIR := data
VENV := .venv

# Source file locations
C_SOURCES := $(wildcard $(C_SRC_DIR)/*.c)
PY_SCRIPTS := $(wildcard $(PY_SRC_DIR)/*.py)
GRAPH_SOURCE := $(DEV_DIR)/voronoi_visual.c
GRAPH_DATA_FILE := $(DATA_DIR)/voronoi_data.txt
REQ_FILE := requirements.txt

# Timestamp and log files
TIMESTAMP:= $(shell date +%m-%d-%Y_%H-%M-%S)
LOG_FILE := $(LOG_DIR)/run_$(TIMESTAMP).log

# Output binaries from C files
C_BINARIES := $(patsubst $(C_SRC_DIR)/%.c, $(BIN_DIR)/%, $(C_SOURCES))
GRAPH_BINARY := $(BIN_DIR)/voronoi_visual

# Compiler settings
CC := gcc
CFLAGS := -O3 -fopenmp -lm

# Python virtual environment
PYTHON := $(VENV)/bin/python
PIP := $(VENV)/bin/pip

# Default target
all: venv build run

# Target for visualization
graph: graph_build graph_run

# -------------------------------------------------------------------
# Virtual environment setup
# -------------------------------------------------------------------
venv:
	@if [ ! -d $(VENV) ]; then \
		echo "Creating virtual environment with virtualenv..."; \
		python3 -m virtualenv $(VENV); \
		echo "Installing dependencies..."; \
		$(PIP) install --upgrade pip; \
		$(PIP) install -r $(REQ_FILE); \
		echo "\n"; \
	else \
		echo "Virtual environment already exists.\n"; \
	fi

# -------------------------------------------------------------------
# Build C programs
# -------------------------------------------------------------------
build: $(C_BINARIES)
$(BIN_DIR)/%: $(C_SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) $< -o $@ 

# -------------------------------------------------------------------
# Hardware Info - Append system info to log file
# -------------------------------------------------------------------
info:
	@mkdir -p $(LOG_DIR)
	@echo "============================================================" > $(LOG_FILE)
	@echo " VORONOI PERFORMANCE BENCHMARK LOG" >> $(LOG_FILE)
	@echo " Date: $(shell date)" >> $(LOG_FILE)
	@echo " Output file: $(LOG_FILE)" >> $(LOG_FILE)
	@echo "============================================================" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)

	@echo "--------------------[ PYNQ-Z1 SYSTEM INFORMATION ]--------------------" >> $(LOG_FILE)
	@echo "FPGA Chip:          Xilinx Zynq-7020 (xc7z020clg400-1)" >> $(LOG_FILE)
	@echo "CPU (PS):           ARM Cortex-A9 (dual-core)" >> $(LOG_FILE)
	@echo "PS Clock Speed:     650 MHz" >> $(LOG_FILE)
	@echo "PL Clock Speed:     100 MHz (default FCLK_CLK0)" >> $(LOG_FILE)
	@echo "Cores:              2" >> $(LOG_FILE)
	@echo "Threads per Core:   1" >> $(LOG_FILE)
	@echo "Memory:             512 MiB (shared between PS and PL)" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)

	@echo "--------------------[ HOST MACHINE INFORMATION ]--------------------" >> $(LOG_FILE)
	@echo "Hostname:           $(shell hostname)" >> $(LOG_FILE)
	@echo "CPU:                $$(lscpu | grep 'Model name' | awk -F: '{print $$2}' | sed 's/^ *//')" >> $(LOG_FILE)
	@echo "Clock Speed:        $$(awk -F': ' '/cpu MHz/ {print $$2; exit}' /proc/cpuinfo) MHz" >> $(LOG_FILE)
	@echo "Cores:              $$(lscpu | grep '^CPU(s):' | awk -F: '{print $$2}' | sed 's/^ *//')" >> $(LOG_FILE)
	@echo "Threads per Core:   $$(lscpu | grep 'Thread(s) per core' | awk -F: '{print $$2}' | sed 's/^ *//')" >> $(LOG_FILE)
	@echo "Memory:             $$(free -h | grep Mem: | awk '{print $$2}')" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)


# -------------------------------------------------------------------
# Run all C programs and Python scripts (logs to file)
# -------------------------------------------------------------------
run: info
	@echo "--------------------[ PYNQ BENCHMARK RESULTS ]---------------------" >> $(LOG_FILE)
	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "| PYNQ VORONOI IMPLEMENTATION |" >> $(LOG_FILE)
	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "Array size: $(PYNQ_SIZE)" >> $(LOG_FILE)
	@echo "[PYNQ] pynq_voronoi_hls.cpp: $(PYNQ_TIME) seconds" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)

	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "| PARALLEL C (CPU) VORONOI IMPLEMENTATION |" >> $(LOG_FILE)
	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "Array size: $(PYNQ_SIZE)" >> $(LOG_FILE)
	@echo "[PYNQ] pynq_voronoi_omp.c: $(PYNQ_OMP_C_TIME) seconds" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)

	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "| SEQUENTIAL C VORONOI IMPLEMENTATION |" >> $(LOG_FILE)
	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "Array size: $(PYNQ_SIZE)" >> $(LOG_FILE)
	@echo "[PYNQ] pynq_voronoi_seq_c.c: $(PYNQ_SEQ_C_TIME) seconds" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)
	
	@echo "--------------------[ PERSONAL MACHINE BENCHMARK RESULTS ]---------------------" >> $(LOG_FILE)
	@$(foreach bin, $(C_BINARIES), \
		name=$$(basename $(notdir $(bin))); \
		echo "> Running $$name.c..." | tee -a $(LOG_FILE); \
		if [ "$$name" = "voronoi_omp" ]; then \
			OMP_NUM_THREADS=16 ./$(bin) | tee -a $(LOG_FILE); \
		else \
			./$(bin) | tee -a $(LOG_FILE); \
		fi; \
		echo "" | tee -a $(LOG_FILE); \
	)
	@$(foreach py, $(PY_SCRIPTS), \
		echo "> Running $(notdir $(py))..." | tee -a $(LOG_FILE); \
		$(PYTHON) $(py) | tee -a $(LOG_FILE); \
		echo "" | tee -a $(LOG_FILE); \
	)

# -------------------------------------------------------------------
# Compile C visualization code
# -------------------------------------------------------------------
graph_build: $(GRAPH_BINARY)
$(GRAPH_BINARY): $(GRAPH_SOURCE)
	@mkdir -p $(BIN_DIR)
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) $< -o $@

# -------------------------------------------------------------------
# Run C visualization binary
# -------------------------------------------------------------------
graph_run: 
	@mkdir -p $(DATA_DIR)
	@touch $(GRAPH_DATA_FILE)
	@echo "$$(OMP_NUM_THREADS=16 ./$(GRAPH_BINARY))"

# -------------------------------------------------------------------
# Clean project: remove binaries, logs, and virtual environment
# -------------------------------------------------------------------
clean: clean_venv clean_bin clean_logs clean_gnu

# -------------------------------------------------------------------
# Remove virtual environment
# -------------------------------------------------------------------
clean_venv:
	@echo "Removing virtual environment..."
	@rm -rf $(VENV)

# -------------------------------------------------------------------
# Remove binaries
# -------------------------------------------------------------------
clean_bin:
	@echo "Removing binaries..."
	@rm -rf $(BIN_DIR)

# -------------------------------------------------------------------
# Remove logs
# -------------------------------------------------------------------
clean_logs:
	@echo "Removing logs..."
	@rm -rf $(LOG_DIR)

# -------------------------------------------------------------------
# Remove gnuplot data
# -------------------------------------------------------------------
clean_gnu:
	@echo "Removing gnuplot data..."
	@rm -rf $(DATA_DIR)