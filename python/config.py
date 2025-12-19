# -------------------------------------------------------------------
# config.py: configuration module
# handles config.ini files I/O
# global constants and parameters

import os


#global
INI_LINES = 20 # number of data lines in file
INI_DATA_LENGTH = 10 # length of data in each line
NUM_OF_EXP = 30 # > 0
W_EXTREME = 1.0
bl1_EXTREME = 1.0
X_EXTREME = 1 # in c_engine, the x-interval
TEST_SIZE = 40 # number of point to predict
# set absolute directory
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
C_ENGINE_DIR = os.path.join(BASE_DIR, "..", "c_engine")
DATA_DIR = os.path.join(BASE_DIR, "..", "data")
# set absolute file paths
FILE_CONFIG = os.path.join(DATA_DIR, "config.ini")
FILE_CONFIG_1ST = os.path.join(DATA_DIR, "config_1st.ini")
FILE_C_ENGINE = os.path.join(C_ENGINE_DIR, "nnfit")
FILE_DB = os.path.join(DATA_DIR, "nnfit.db")





# -------------------------------------------------------------------
def read(file_name):
    '''
    load config.ini parameters into dictionary 'ini_data'
    '''
    str_data = []
    with open(file_name, mode='r', newline='\n') as ini_file:
        ini_file.readline() # title
        ini_file.readline()
        # data part of line goes to string 'str_data'
        for i in range(INI_LINES):
            str_data.append(ini_file.read(INI_DATA_LENGTH).strip())
            ini_file.readline() # comment part ('//') is eliminates 
    
    ini_data = {"new_ts": 0, "ts_size": 0, "mb": 0, "fx": 0, "a": 0, 
            "b": 0, "c": 0, "eta": 0, "epoch_num": 0, "delta": 0, 
            "w00l1": 0, "w10l1": 0, "w20l1": 0, "w00l2": 0, 
            "w01l2": 0, "w02l2": 0, "b0l1": 0, "b1l1": 0, 
            "b2l1": 0, "b0l2": 0}
    i = 0
    # ini data save into dictionary 'data' as true data
    for key in ini_data:
        if str_data[i].isalpha():
            ini_data[key] = str_data[i]
        elif str_data[i].isdigit():
            ini_data[key] = int(str_data[i])
        else:
            ini_data[key] = float(str_data[i])
        i = i + 1

    return ini_data

# -------------------------------------------------------------------
def update(ini_data, file_name):
    '''
    creates a config.ini file called 'file_name' using the dictionary data
    
    :param data (dict): has all data to load into config.ini file
    :param file_name: config.ini name
    '''
    labels = {'new_ts': '// generate new TS? Y/N [bool ini_d.flag_genTS]',
             'ts_size': '// TS size [int ini_d.ts_size]',
             'mb': '// mini-batch size [int ini_d.mb_size] < TS size',
             'fx': '// choose fx: (A) quad, (B) linear (C) cosine [char ini_d.fx_choice]',
             'a': '// a [float ini_d.fx_a]', 
             'b': '// b [float ini_d.fx_b]', 
             'c': '// c [float ini_d.fx_c]', 
             'eta': '// learning rate [double ini_d.eta]',
             'epoch_num': '// epoch number [int ini_d.epoch_number]',
             'delta': '// delta, threshold value of C to stop grad-desc [double ini_d.delta]', 
             'w00l1': '// w_{00}^{(1)} layer 1 (hidden) weight 00',
             'w10l1': '// w_{10}^{(1)}', 
             'w20l1': '// w_{20}^{(1)} all three -> [float ini_d.wl1[3]]', 
             'w00l2': '// w_{00}^{(2)} layer 2 (output) weight 00',
             'w01l2': '// w_{01}^{(2)}', 
             'w02l2': '// w_{02}^{(2)} all three -> [float ini_d.wl2[3]]', 
             'b0l1': '// b_0^{(1)} layer 1 bias [float ini_d.b0l1]', 
             'b1l1': '// b_1^{(1)}', 
             'b2l1': '// b_2^{(1)} all three -> [float ini_d.bl1[3]]', 
             'b0l2': '// b_0^{(2)} layer 2 bias [float ini_d.bl2]'}
    str_ini_data = []
    i = 0
    # finds the matching keys in 'data' and 'labels'
    # and copy data[key]+value into 'str_data'
    for key, value in labels.items():
        if isinstance(ini_data[key], str):
            temp_str = str(ini_data[key]).ljust(INI_DATA_LENGTH, ' ')
        else:
            temp_str = str(f"{ini_data[key]}").ljust(INI_DATA_LENGTH, ' ')
        str_ini_data.append(temp_str + value)
        
    with open(file_name, mode='w', newline='\n') as ini_file:
        ini_file.write("DATA      // COMMENT \n") # title
        ini_file.write("----------------------------------"     \
            "-----------------------------------------------\n")
        # real data
        for line_data in str_ini_data:
            ini_file.write(line_data + "\n")

    
if __name__ == '__main__':
    None
