# -------------------------------------------------------------------
# main.py: orchestrator module
# pipelines the execution of all modules

import os

import db
import config
import runner
import experiments
import analysis

def main():
    '''
    Orchestrator:
    
    Before running, two config.ini must exist:
        - config_1st.ini: initial parameters chosen by 
        the user. Does not count as an experiment. 
        new_ts = Y: forces c_engine to create training set

        - config.ini: dynamic file, modified in each
        experiment. new_ts = N: All experiments belong
        to the same training set
    
    By default, the c_engine is compiled
    '''
    os.system('cls||clear')
    

    # ----------------- initialization run
    # reads config_1st.ini always with new_ts = 'Y'
    print()
    print("initializing...\n")
    db.create_tables()
    ini_data = config.read(config.FILE_CONFIG_1ST) 
    # compiles c_engine
    runner.compile_c(config.C_ENGINE_DIR)
    # first run: to initialize xfx table
    runner.run_c(config.FILE_C_ENGINE)
    # updates only new_TS = 'N' in ini_data
    ini_data['new_ts'] = 'N'
    config.update(ini_data, config.FILE_CONFIG)


    # ----------------- experimental runs
    # list of ini_data dictionaries
    ini_data_all_points = experiments.generate_wb_points(
        config.NUM_OF_EXP, config.W_EXTREME, config.bl1_EXTREME)
    # runs experiments from each wb_data_points
    i = 0
    print("experiment number:   ", end = "")
    for ini_data in ini_data_all_points:
        # print progress 
        print(f"\b\b\b{i:3}", end = "", flush = True)
        # updates config.ini
        config.update(ini_data, config.FILE_CONFIG) 
        # stores new ini_data in db
        exp_id = db.save_experiment(ini_data) 
        # runs c_engine and capture stdout: w, b, loss
        stdout = runner.run_c(config.FILE_C_ENGINE) 
        # stores wb at the end of gradient descent
        opt_wb = db.save_optimal_wb(stdout, exp_id)
        # stores loss from stdout in db table loss
        db.save_loss(stdout, exp_id)
        # predicts using the optimal (last) set of wb
        predicts = experiments.predictions(opt_wb, config.TEST_SIZE, exp_id)
        # store predictions using the optimal (last) set of wb
        db.save_predictions(predicts, exp_id)
        i = i + 1
    print("")
    
    # ----------------- analysis and visualization
    # among all experiments, find the lowest loss
    print("")
    best_exp = analysis.find_min_loss()
    # loss curve
    print("")
    analysis.plot_loss(best_exp)
    # function curve
    analysis.plot_function(best_exp)
    # script ended
    input("Finished! Press Enter to exit and close the plot.")
    


if __name__ == '__main__':
    main()
     
