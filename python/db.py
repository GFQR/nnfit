# -------------------------------------------------------------------
# db.py: database module
# handles SQL I/O
# one db with 5 tables: 'xfx', 'ini', 'predictions', 'optimal_wb' and 'loss'

import sqlite3
import config


# ----------------------------------------------------------------    
def create_tables():
    '''
    Creates the necessary tables in the SQLite database for storing experiment data.

    Tables created:
        - xfx: Stores x and fx values with an auto-incrementing primary key.
        - ini: Stores experiment initialization parameters and metadata.
        - predictions: Stores prediction results, referencing the ini table.
        - loss: Stores loss values, referencing the ini table.

    The function connects to the database specified by `db_name`, executes the SQL
    statements to create the tables if they do not exist, and commits the changes.
    Any SQLite errors encountered during execution are printed to the console.
    '''
    sql_qry_xfx = """
        CREATE TABLE IF NOT EXISTS xfx (
            id INTEGER PRIMARY KEY, 
            x REAL, fx REAL
            );
    """
    sql_qry_ini = """
        CREATE TABLE IF NOT EXISTS ini (
            id INTEGER PRIMARY KEY, 
            new_ts TEXT, ts_size INTEGER, mb INTEGER, 
            fx TEXT, a REAL, b REAL, c REAL, 
            eta REAL, epoch_num INTEGER, delta REAL, 
            w00l1 REAL, w10l1 REAL, w20l1 REAL, 
            w00l2 REAL, w01l2 REAL, w02l2 REAL, 
            b0l1 REAL, b1l1 REAL, b2l1 REAL, b0l2 REAL 
            );
    """
    sql_qry_optimal = """
        CREATE TABLE IF NOT EXISTS optimal_wb (
            id INTEGER PRIMARY KEY, 
            exp_id INTEGER, 
            w00l1 REAL, w10l1 REAL, w20l1 REAL, 
            w00l2 REAL, w01l2 REAL, w02l2 REAL, 
            b0l1 REAL, b1l1 REAL, b2l1 REAL, b0l2 REAL, 
            FOREIGN KEY(exp_id) REFERENCES ini(id)
            );
    """
    sql_qry_loss = """
        CREATE TABLE IF NOT EXISTS loss (
            id INTEGER PRIMARY KEY, 
            exp_id INTEGER, 
            epoch INTEGER, 
            mse REAL, 
            FOREIGN KEY(exp_id) REFERENCES ini(id)
            );
    """
    sql_qry_pred = """
        CREATE TABLE IF NOT EXISTS predictions (
            id INTEGER PRIMARY KEY, 
            exp_id INTEGER, x REAL, fx_pred REAL, 
            FOREIGN KEY(exp_id) REFERENCES ini(id)
            );
    """
    # Connect to the database
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            db.execute(sql_qry_xfx)
            db.execute(sql_qry_ini)
            db.execute(sql_qry_optimal)
            db.execute(sql_qry_pred)
            db.execute(sql_qry_loss)
            db.commit()
    except sqlite3.Error as e:
        print(e)

# ----------------------------------------------------------------    
def save_experiment(ini_data):
    '''
    Inserts a new experiment record into the 'ini' table of the SQLite database.

    Parameters:
        ini_data (dict): A dictionary containing the experiment data to be inserted. 
            The keys should match the column names in the 'ini' table.

    Raises:
        sqlite3.Error: If a database error occurs during insertion.

    Notes:
        - The function expects the global variable 'db_name' to be defined as the 
            database file path.
        - The order of values in 'ini_data' must correspond to the columns 
            specified in the SQL query.
    '''
    sql_qry_ins = """
    INSERT INTO ini (
        new_ts, ts_size, mb, fx, a, b, c, 
        eta, epoch_num, delta, w00l1, w10l1, 
        w20l1, w00l2, w01l2, w02l2, b0l1, b1l1, 
        b2l1, b0l2
        ) 
        VALUES (
        ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 
        ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
        )
    """
    # tuple with all parameters to insert
    correct_order_cols = ['new_ts', 'ts_size', 'mb', 'fx', 'a', 'b', 'c', 
                          'eta', 'epoch_num', 'delta', 'w00l1', 'w10l1', 
                          'w20l1', 'w00l2', 'w01l2', 'w02l2', 'b0l1', 
                          'b1l1', 'b2l1', 'b0l2'
                          ]
    param = tuple(ini_data[col] for col in correct_order_cols)
   # Connect to the database
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            cursor.execute(sql_qry_ins, param)
            # extract exp_id of last inserted experiment
            exp_id = cursor.lastrowid
            db.commit()
    except sqlite3.Error as e:
        print(e)

    return exp_id

# ----------------------------------------------------------------    
def save_optimal_wb(stdout, exp_id):
    '''
    saves the optimized weights and biases that result for an
    experiment
    
    :param stdout (dict): output from c_engine
    :param exp_id: experiment id
    '''
    sql_qry_ins = """
        INSERT INTO optimal_wb (
            exp_id,
            w00l1, w10l1, w20l1, 
            w00l2, w01l2, w02l2, 
            b0l1, b1l1, b2l1, 
            b0l2 
            ) 
            VALUES (
            ?, ?, ?, ?, ?, ?,
            ?, ?, ?, ?, ?
            );
    """
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            opt_wb = [exp_id]
            # preparing list opt_wb to insert in db
            for key in ['w_layer_1', 'w_layer_2', 
                        'b_layer_1']:
                    opt_wb.extend(list(stdout['weights'][key]))
            opt_wb.append(stdout['weights']['b_layer_2'])
            cursor.execute(sql_qry_ins, opt_wb)
            db.commit()
            return opt_wb
    except sqlite3.Error as e:
            print("db:", e)


# ----------------------------------------------------------------    
def save_loss(stdout, exp_id):
    '''
    save loss(epoch) data into table 'loss' corresponding 
    to the exp_id = exp_num
    
    stdout (dict): from c_engine with w, b and loss
    exp_id: experiment number corresponging to id - table ini
    '''
    sql_qry_ins = """
        INSERT INTO loss (
            exp_id, epoch, mse
            ) 
            VALUES (
            ?, ?, ?
            );
    """
    # print(stdout)
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            for epoch, loss in stdout["loss"].items():
                cursor.execute(sql_qry_ins, (exp_id, epoch, loss))
            db.commit()
    except sqlite3.Error as e:
            print(e)

# ----------------------------------------------------------------    
def save_predictions(predictions, exp_id):
    '''
    save predictions' data into table 'predictions'
    
    predictions (list of dicts): [{x: fx, ...}]
    exp_id: experiment id number corresponging to id - table ini
    '''
    sql_qry_ins = """
        INSERT INTO predictions (
            exp_id, x, fx_pred
            ) 
            VALUES (
            ?, ?, ?
            );
    """
    try:
        with sqlite3.connect(config.FILE_DB) as db:
            cursor = db.cursor()
            for dict in predictions:
                cursor.execute(
                    sql_qry_ins, (
                        exp_id, 
                        list(dict.keys())[0], 
                        list(dict.values())[0]
                    )
                )
            db.commit()
            '''
            for x, fx_pred in predictions.items():
                cursor.execute(sql_qry_ins, (exp_id, x, fx_pred))
            db.commit()
            '''
    except sqlite3.Error as e:
            print(e)


if __name__ == '__main__':
    None
