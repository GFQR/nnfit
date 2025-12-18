# -------------------------------------------------------------------
# runner.py: run module 
# compiles and runs c_engine

import subprocess
import json

#global

# -------------------------------------------------------------------
def compile_c(file_dir):
    '''
    compile the c_engine with Makefile
    '''
    try:
        result = subprocess.run(["make"], capture_output=True, \
                                text=True, cwd=file_dir, check=True)
    except Exception as e:
        print("compile_C: ", e)
        exit()
 

# -------------------------------------------------------------------
def run_c(file_name):
    '''
    run the c_engine 'file_name' and capture stdout JSON data
    of the type:
    {
        "weights": {
            "w_layer_1": [0.03896, -1.76536, 1.73470],
            "b_layer_1": [-0.04918, -0.35667, -0.34340],
            "w_layer_2": [-0.042738, 1.014976, 1.029441],
            "b_layer_2": -0.266997
        },
        "loss": {
            "0": 0.039609,
            "1": 0.002054,
            ...  
        }
    }
    '''
    try:
        result = subprocess.run([file_name], capture_output=True, text=True)
    except FileNotFoundError:
        print("File not found.")
        exit()

    return json.loads(result.stdout)

if __name__ == '__main__':
    None
