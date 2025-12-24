# NNFit — A Neural Network Fitting Engine in C with Python Orchestration and SQLite Experiments

## Overview

**NNFit** is a compact machine learning project designed to demonstrate a deep, mechanistic understanding of neural networks and experimental workflows.

The core training loop is implemented **from scratch in C**, including forward propagation, backpropagation, and stochastic gradient descent. A **Python orchestration layer** manages experiment configuration, execution, data persistence, and visualization. All experiment metadata and results are stored in a **SQLite database**, enabling reproducibility and systematic comparison across runs.

This project intentionally avoids high-level ML frameworks in order to expose and control every step of the learning process.

## Purpose

NNFit is a from-scratch neural network framework designed to approximate real-valued functions of a single variable.

The project focuses on:
- explicit implementation of forward and backward propagation,
- controlled experimentation with initialization and hyperparameters,
- systematic comparison of training runs stored in a relational database.

The goal is not raw performance, but **mechanistic understanding, reproducibility, and experimental rigor**.


## Project Architecture

The project is structured as a modular pipeline:

### C engine (`c_engine/`)
- Implements a fully connected 1-3-1 neural network with ReLU hidden units
- Performs training via stochastic gradient descent
- Reads hyperparameters and initialization from an `.ini` configuration file
- Emits final weights, biases, and per-epoch loss via structured JSON on `stdout`

### Python layer (`python/`)
- Orchestrates multiple experiments
- Compiles and launches the C engine as a subprocess
- Parses JSON output and persists results
- Generates plots for predictions and convergence

### SQLite database (`data/`, `examples/`)

- Stores training data, experiment configurations, loss curves, optimal parameters, and predictions
- Enables one-to-many relationships between experiments and results
- Supports reproducible and auditable experimentation

## Experiments and Results

The repository includes five fully reproducible examples:

- 2 linear functions
- 2 quadratic functions
- 1 oscillatory (cosine) function

Each example contains:

- The original configuration file (config.ini)
- The training database with all experiments
- Plots of:
    - Model prediction vs. ground truth
    - Loss as a function of epoch
- Captured standard output from the python

For each case, multiple runs with different initial conditions are performed.
The experiment with the lowest final loss is automatically selected and visualized.

## Why This Project

This project is intended to demonstrate:

- Understanding of neural networks __beyond library usage__
- Comfort working across __C, Python, and SQL__
- Experimental rigor (initial conditions, convergence, failure modes)
- Clean separation between computation, orchestration, and analysis
- Reproducibility and traceability of ML experiments

## How to Run

First, define the target function and global parameters in `data/config_1st.ini`. Then run the Python orchestrator:

```
cd python
python3 main.py
```

The Python orchestrator will:

- Initialize the suite,
- configure multiple runs,
- execute the C engine on each experiment,
- store all results in the SQLite database,
- Analyze outcomes and generate plots.

Optimal weights and biases, along with full experiment metadata, are stored in `nnfit.db`.

## Notes

- The C engine can be executed independently. Its output (loss trajectory and optimal parameters) is emitted as JSON at the end of execution.

- Module-specific documentation is available within each subdirectory.

## License

This project is released under the MIT License.
