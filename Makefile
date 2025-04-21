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
PYNQ_TIME := 0.605
#####################################################################################################

# Project directories
SRC_DIR := src
C_SRC_DIR := $(SRC_DIR)/c
PY_SRC_DIR := $(SRC_DIR)/python
BIN_DIR := bin
LOG_DIR := logs
DATA_DIR := data
VENV := .venv

# Source file locations
C_SOURCES := $(wildcard $(C_SRC_DIR)/*.c)
PY_SCRIPTS := $(wildcard $(PY_SRC_DIR)/*.py)
REQ_FILE := requirements.txt
GRAPH_SOURCE := voronoi_randomseeds.c

# Timestamp and log files
TIMESTAMP:= $(shell date +%m-%d-%Y_%H-%M-%S)
LOG_FILE := $(LOG_DIR)/run_$(TIMESTAMP).log

# Output binaries from C files
C_BINARIES := $(patsubst $(C_SRC_DIR)/%.c, $(BIN_DIR)/%, $(C_SOURCES))
GRAPH_BINARY := voronoi_randomseeds

# Compiler settings
CC := gcc
CFLAGS := -O2 -fopenmp -lm

# Python virtual environment
PYTHON := $(VENV)/bin/python
PIP := $(VENV)/bin/pip

# Default target
all: venv build run

# -------------------------------------------------------------------
# Virtual environment setup
# -------------------------------------------------------------------
venv:
	@if [ ! -d $(VENV) ]; then \
		echo "Creating virtual environment with virtualenv..."; \
		python3 -m virtualenv $(VENV); \
		echo "Installing dependencies..."; \
		$(PIP) install --upgrade pip > /dev/null; \
		$(PIP) install -r $(REQ_FILE) > /dev/null; \
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

	@echo "--------------------[ SYSTEM INFORMATION ]--------------------" >> $(LOG_FILE)
	@echo "Hostname: $(shell hostname)" >> $(LOG_FILE)
	@echo "CPU: $$(lscpu | grep 'Model name' | awk -F: '{print $$2}' | sed 's/^ *//')" >> $(LOG_FILE)
	@echo "Cores: $$(lscpu | grep '^CPU(s):' | awk -F: '{print $$2}' | sed 's/^ *//')" >> $(LOG_FILE)
	@echo "Threads per core: $$(lscpu | grep 'Thread(s) per core' | awk -F: '{print $$2}' | sed 's/^ *//')" >> $(LOG_FILE)
	@echo "Memory: $$(free -h | grep Mem: | awk '{print $$2}')" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)

# -------------------------------------------------------------------
# Run all C programs and Python scripts (logs to file)
# -------------------------------------------------------------------
run: info
	@echo "--------------------[ BENCHMARK RESULTS ]---------------------" >> $(LOG_FILE)
	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "| PYNQ VORONOI IMPLEMENTATION |" >> $(LOG_FILE)
	@echo "-------------------------------" >> $(LOG_FILE)
	@echo "Array size: $(PYNQ_SIZE)" >> $(LOG_FILE)
	@echo "[PYNQ] voronoi_pynq: $(PYNQ_TIME) seconds" >> $(LOG_FILE)
	@echo "" >> $(LOG_FILE)
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
# Run all C programs and Python scripts (logs to file)
# -------------------------------------------------------------------
graph: $(C_BINARIES)
$(BIN_DIR)/%: $(C_SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) $< -o $@

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