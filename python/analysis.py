# -------------------------------------------------------------------
# analysis.py: analysis and visualization module

import matplotlib # type: ignore
# Set the interactive backend: requires system-wide installation of 
# sudo apt install python3-tk, out of venv
matplotlib.use('TkAgg') # TkAgg is often pre-installed and reliable

import matplotlib.pyplot as plt # type: ignore 
import numpy as np
import math

import sqlite3
import config
import db

# -------------------------------------------------------------------
def find_min_loss():
    '''
    find the experiment with minimum loss in table 'loss'
    return exp_id of best experiment 
    '''
    sql_qry_sel01 = '''
        SELECT l.exp_id, l.mse
        FROM loss l
        WHERE l.mse = (
            SELECT MIN(mse) 
            FROM loss);
    '''
    with sqlite3.connect(config.FILE_DB) as db:
        cursor = db.cursor()
        cursor.execute(sql_qry_sel01)
        row = cursor.fetchone()
    
    print("Best experiment:")
    print(f"exp_id = {row[0]} MSE = {row[1]}")
    
    return row[0]

# -------------------------------------------------------------------
def plot_loss(best_exp):
    '''
    plot loss vs epoch for experiment 'best_exp' as defined 
    by lowest MSE in table 'loss'
    '''
    sql_qry_sel01 = '''
        SELECT l.epoch, l.mse
        FROM loss l
        WHERE l.exp_id = ?;
    '''
    with sqlite3.connect(config.FILE_DB) as db:
        cursor = db.cursor()
        cursor.execute(sql_qry_sel01, (best_exp,))
        row = cursor.fetchall()
    
    # Create the plot
    # define argument and function
    epoch = np.array(tuple(i0[0] for i0 in row))
    loss = np.array(tuple(i1[1] for i1 in row))
    plt.figure(figsize=(8, 6)) 
    plt.plot(epoch, loss, label='Loss', color='blue')
    # titles and labels
    plt.title('LOSS')
    plt.xlabel('epoch')
    plt.ylabel('loss')
    # grid and legend
    plt.grid(True)
    plt.legend()
    # Display
    plt.show(block=False)

    return row

# -------------------------------------------------------------------
def plot_function(best_exp):
    '''
    use table predictions + the function itself from table ini
    to plot both analytical function and predictions
    '''
    # read function and function parameters
    sql_qry_sel01 = '''
        SELECT i.fx, i.a, i.b, i.c
        FROM ini i
        WHERE i.id = ?;
    '''
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            cursor.execute(sql_qry_sel01, (best_exp,))
            row_fx = cursor.fetchall()
    except Exception as e:
        print(e)
    # read predictions 
    sql_qry_sel02 = '''
        SELECT p.x, p.fx_pred
        FROM predictions p
        WHERE p.exp_id = ?;
    '''
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            cursor.execute(sql_qry_sel02, (best_exp,))
            row_pred = cursor.fetchall()
    except Exception as e:
        print(e)

    # prepare analytic results
    x_analyt = np.linspace(-1,1,100)
    if row_fx[0][0] == "A":
        f_analyt = row_fx[0][1]*(x_analyt - row_fx[0][2])* \
            (x_analyt - row_fx[0][2]) + row_fx[0][3]
    elif row_fx[0][0] == "B":
        f_analyt = row_fx[0][1]*x_analyt + row_fx[0][2]
    elif row_fx[0][0] == "C":
        f_analyt = row_fx[0][1]*np.cos(row_fx[0][2]*x_analyt) \
            + row_fx[0][3]
    # prepare predictions' results
    x = np.array(tuple(r[0] for r in row_pred))
    fx_pred = np.array(tuple(r[1] for r in row_pred))

    # create plot
    plt.figure(figsize=(8, 6)) 
    # Create the analytic line plot
    plt.plot(x_analyt, f_analyt, label='f(x)', color='red')
    # Create the predictions' scattered plot
    plt.scatter(x, fx_pred, label='f_pred', color='blue')
    # titles and labels
    plt.title('f(x)')
    plt.xlabel('x')
    plt.ylabel('f(x)')
    # grid and legend
    plt.grid(True)
    plt.legend()
    # Display
    plt.show(block=False)


if __name__ == '__main__':
    None