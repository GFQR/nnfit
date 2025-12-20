# DOCUMENTATION FOR THE PYTHON MODULES

The Python modules are responsible for orchestrating multiple experiments, storing experiment data, performing analysis, and visualizing results.

## Orchestrator

script: `main.py`

Coordinates the full workflow of the project, including configuring experiments, launching the C engine, collecting results, and triggering analysis and visualization.

## System configuration

script: `config.py`

Stores global data and provides utilities to read and update `config*.ini` files used by the C engine.

__IMPORTANT__: The user may modify:

- The number of experiments via the global variable `NUM_OF_EXP`
- The range of random variation for weights and biases via `W_EXTREME` and `bl1_EXTREME` (see "Experiments").
- the size of the test set via `TEST_SIZE` (see "Experiments").

## Database handling

script: `db.py`

Manages all SQLite I/O operations to the `nnfit.db` database, including storage of experiment configurations, loss curves, optimal parameters, and predictions.

## C engine controller 

script: `runner.py`

Provides the C–Python interface used to compile and execute the C engine as a subprocess, and to capture its structured output.

## Experiments

script: `experiments.py`

Generates multiple experiments by sampling initial weights and biases randomly within bounds defined by `W_EXTREME` and `bl1_EXTREME` (see "System configuration").

Also generates predictions on a test set of size `TEST_SIZE` for each experiment (see "System configuration").

## Analysis and visualization

Identifies the best experiment, defined as the one achieving the lowest final loss. Produces plots of:

- Loss versus epoch
- Analytical reference function $f(x)$
- Neural network predictions

