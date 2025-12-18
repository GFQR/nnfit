# Examples — Function Approximation Cases

This directory contains fully reproducible experiment cases demonstrating the behavior of the neural network when approximating different classes of one-dimensional functions.

## Target Functions

The following function families are considered:

### Case A — Quadratic

f(x) = a(x − b)² + c

### Case B — Linear

f(x) = a·x + b

### Case C — Oscillatory

f(x) = a·cos(b·x) + c


## Experiment Contents

Each example directory (e.g., `A_01`, `B_02`, `C_01`) contains:

- Initial and working configuration files: `config_1st.ini` and `config.ini`.
- A SQLite database (`nnfit.db`) containing:
  - traning set,
  - experiment parameters,
  - training loss curves,
  - optimal weights and biases,
  - predictions on the evaluation set
- Generated plots:
  - model prediction vs. ground truth,
  - loss as a function of training epoch
- Captured standard output from the Python orchestrator

## Notes on Convergence

- Linear and quadratic functions are reliably fitted.
- Oscillatory functions exhibit limitations consistent with the network architecture (1–3–1 with ReLU activations).
- Convergence quality depends on learning rate and mini-batch size, which are adjusted when necessary directly in `data/config_1st.ini`.

## Purpose of These Examples

These cases are intended to illustrate:

- the expressive capacity and limitations of a small neural network,
- sensitivity to initialization and hyperparameters,
- the importance of systematic experimentation in function approximation tasks.
