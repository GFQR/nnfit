# DOCUMENTATION FOR C ENGINE MODULES

## Abstract

This module implements a ___1–3–1 fully connected neural network___ to approximate real-valued functions of a single variable f(x).

The scalar input 
x
x is provided to the input neuron, followed by a hidden layer of three ReLU units and a linear output neuron. The entire training pipeline — forward propagation, backpropagation, and stochastic gradient descent (SGD) — is implemented ___from scratch in C___.

A SQLite database is used to store and manipulate the training set.

## Modules

- `nnfit.c`

    Main entry point. Handles initialization, configuration parsing, dataset handling, the SGD training loop, and printing of results.

- `nnfit_utils.c`

    Implements forward propagation, backpropagation, gradient computation, SQL utilities, and numerical support routines.

- `nnfit.h` 

    Header file containing structures, global variables, constants, and function prototypes.

- `config*.ini`

    Initialization files defining hyperparameters, function parameters, and initial weights and biases. These can be modified directly or via the Python orchestration layer.

- `nnfit.db`

    SQLite database storing the training set and experiment-related tables.

- `Makefile`

    Compilation and linking instructions.

- `libsqlite3.a`

    Static library containing the SQLite C API.

## How to compile and run

First step: Build the static library `libsqlite3.a`, following https://www.ubuntumint.com/create-static-library-linux/. Next use Makefile to compile. Set the desired values in config.ini according to explanation below. Run `./nnfit`.

## Config file

The user should only modify `config_1st.ini`. 

This file defines the initial configuration used for the first experiment, whether run directly via the C executable or through the Python orchestrator. Subsequent experiments use an automatically updated `config.ini`.

## INI file format and usage

```
DATA      // COMMENT 
---------------------------------------------------------------------------------
Y         // generate new TS? Y/N [bool ini_d.flag_genTS]
800       // TS size [int ini_d.ts_size]
3         // mini-batch size [int ini_d.mb_size] < TS size
A         // choose fx: (A) quad, (B) linear (C) cosine [char ini_d.fx_choice]
0.7       // a [float ini_d.fx_a]
0.5       // b [float ini_d.fx_b]
1.0       // c [float ini_d.fx_c]
0.8       // learning rate [double ini_d.eta]
100       // epoch number [int ini_d.epoch_number]
0.0003    // delta, threshold value of C to stop grad-desc [double ini_d.delta]
0.1       // w_{00}^{(1)} layer 1 (hidden) weight 00
-0.2      // w_{10}^{(1)}
0.05      // w_{20}^{(1)} all three -> [float ini_d.wl1[3]]
-0.1      // w_{00}^{(2)} layer 2 (output) weight 00
0.1       // w_{01}^{(2)}
-0.01     // w_{02}^{(2)} all three -> [float ini_d.wl2[3]]
0.01      // b_0^{(1)} layer 1 bias [float ini_d.b0l1]
-0.05     // b_1^{(1)}
0.22      // b_2^{(1)} all three -> [float ini_d.bl1[3]]
0         // b_0^{(2)} layer 2 bias [float ini_d.bl2]
```
- Header line: tells the exact/fixed formatting to followed.
- Line 0: set or updated the training set. 'N' signals the C code to use an already generated db. 'Y' generates a new set. A new db should be generated if the following lines (L) are modified: L1, L3-L6. Changing parameters without regenerating the DB can lead to mismatch between $f(x)$ and stored values.
- Line 1: note that there is no test set for the c-code, because the exact $f(x)$ can be compared to the NN result.
- Line 2: mini-batch size.
- Line 3: choose $f(x)$. quad: $f(x)=a(x-b)^2+c$. linear: $ax+b$. cosine: $a\cos(bx) + c$.
- Line 4-6: set $f(x)$ parameters.
- Line 7: learning rate.
- Line 8: number of epochs.
- Line 9: when the loss function C is below this delta value, the SGD algorithm stops.
- Line 10-19: weights (w) and biases (b): subscript designates neuron connection, superscript is layer. See file `Docs_eqs.pdf` for more details.

## SQL usage

Used to store and manipulate the training set (and other tables).

schema:

```
CREATE TABLE xfx (
            id INTEGER PRIMARY KEY, 
            x REAL, fx REAL
            );
CREATE TABLE ini (
            id INTEGER PRIMARY KEY, 
            new_ts TEXT, ts_size INTEGER, mb INTEGER, 
            fx TEXT, a REAL, b REAL, c REAL, 
            eta REAL, epoch_num INTEGER, delta REAL, 
            w00l1 REAL, w10l1 REAL, w20l1 REAL, 
            w00l2 REAL, w01l2 REAL, w02l2 REAL, 
            b0l1 REAL, b1l1 REAL, b2l1 REAL, b0l2 REAL 
            );
CREATE TABLE optimal_wb (
            id INTEGER PRIMARY KEY, 
            exp_id INTEGER, 
            w00l1 REAL, w10l1 REAL, w20l1 REAL, 
            w00l2 REAL, w01l2 REAL, w02l2 REAL, 
            b0l1 REAL, b1l1 REAL, b2l1 REAL, b0l2 REAL, 
            FOREIGN KEY(exp_id) REFERENCES ini(id)
            );
CREATE TABLE predictions (
            id INTEGER PRIMARY KEY, 
            exp_id INTEGER, x REAL, fx_pred REAL, 
            FOREIGN KEY(exp_id) REFERENCES ini(id)
            );
CREATE TABLE loss (
            id INTEGER PRIMARY KEY, 
            exp_id INTEGER, 
            epoch INTEGER, 
            mse REAL, 
            FOREIGN KEY(exp_id) REFERENCES ini(id)
            );
```

## Running the C-SQL module directly

- Adjust initial weights and hidden-layer biases if convergence is poor. Introducing negative biases helps prevent all ReLU units from saturating.
- Tune hyperparameters in `.ini` file: learning rate, min-batch size, training set size, stopping threshold.
- compile with `make`
- run `./nnfit` 
- study convergence with loss function
- If convergence is unsatisfactory, adjust parameters and rerun.
