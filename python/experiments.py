# -------------------------------------------------------------------
# experiments.py: experiments module
# handles experiments generation and predictions

import random

import config
import db

# -------------------------------------------------------------------
def generate_wb_points(num_of_exps, w_extreme, bl1_extreme):
    '''
    Generates a number of points in parameter space
    :param num_of_exps: the number of points
    :param w_extreme: the boundaries of each w
    :param bl1_extreme: the boundaries of each b^(1)
    '''
    # use rnd numbers to generate set of {w's,b's} sets
    # use extremes to bound the possible {w's,b's}
    wb = [{} for _ in range(num_of_exps)]
    wb_template = {"w00l1": 0, "w10l1": 0, "w20l1": 0, "w00l2": 0, 
        "w01l2": 0, "w02l2": 0, "b0l1": 0, "b1l1": 0, "b2l1": 0 , 
        "b0l2": 0}
    for i in range(num_of_exps):
        # Generate all points {w,b} in parameter-space 
        # and store in list of dict wb=[{exp 1}, {exp 2}, ...]
        for k in wb_template:
            if k == "b0l2": 
                wb[i][k] = 0
            # improve with gauss distribution
            elif k in ("w00l1", "w10l1", "w20l1"):
                wb[i][k] = round(random.uniform(-w_extreme, w_extreme), 4)
            elif k in ("w00l2", "w01l2", "w02l2"):
                wb[i][k] = round(random.uniform(-w_extreme, w_extreme), 4)
            elif k in ( "b0l1", "b1l1", "b2l1"):
                wb[i][k] = round(random.uniform(-bl1_extreme, bl1_extreme), 4)

    # load config.ini into ini_data_original
    ini_data_all_points = [{} for _ in range(num_of_exps)]
    ini_data_original = config.read(config.FILE_CONFIG) 
    # LOOP over all points {w,b} in parameter-space
    for i in range(num_of_exps):
        # concatenates a single (new) ini_data from: wb + ini_data_original
        ini_data = {}
        for k in ini_data_original:
            ini_data[k] = ini_data_original[k]
        for k in wb[i]:
            ini_data[k] = wb[i][k]
        ini_data_all_points[i] = ini_data # store ini_data for each exp in list

    return ini_data_all_points


# -------------------------------------------------------------------
def predictions(opt_wb, test_size, exp_id):
    '''
    Performs predictions using the given ini_data parameters on
    a test set of size test_size
    :param ini_data: dictionary with all parameters
    :param test_size: size of test set
    :return predictions (dict): keys are x, values are fx_predicted
    '''
    # model parameters
    w00l1 = opt_wb[1]
    w10l1 = opt_wb[2]
    w20l1 = opt_wb[3]
    w00l2 = opt_wb[4]
    w01l2 = opt_wb[5]
    w02l2 = opt_wb[6]
    b0l1 = opt_wb[7]
    b1l1 = opt_wb[8]
    b2l1 = opt_wb[9]
    b0l2 =opt_wb[10]
    predictions = [{} for _ in range(test_size)] # [{x:fx_pred}, ...}]
    for i in range(test_size):
        x = round(random.uniform(-config.X_EXTREME, config.X_EXTREME), 4)
        a0l1 = ReLu(w00l1*x + b0l1)
        a1l1 = ReLu(w10l1*x + b1l1)
        a2l1 = ReLu(w20l1*x + b2l1)
        # forward propagation
        fx_pred = round(w00l2*a0l1 + w01l2*a1l1 + w02l2*a2l1 + b0l2, 4)
        predictions[i][x] = fx_pred
    
    return predictions
    

# -------------------------------------------------------------------
def ReLu(z):
    '''
    ReLu activation function
    '''
    if z <= 0:
        z = 0
    
    return z

if __name__ == '__main__':
    None