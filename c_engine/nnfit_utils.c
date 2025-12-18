/*
CODE
Functions for the nnfit project
*/

#include "nnfit.h"

#define X_EXTREME 1 // interval [-X_EXTREME, +X_EXTREME]
#define DB_NAME "../data/nnfit.db"
#define FILE_CONFIG "../data/config.ini"

// global variables taken from init file
int ts_size = 0; 
int mb_size = 0; 
double eta = 0;
int epoch_number = 0; 
double delta = 0;

// ---------------------------------------------
// INITIALIZATION
// ---------------------------------------------

// ---------------------------------------------
// does whole initialization process
model_param init(void)
{
    // random seed can improve with arc4random()
    init_rseed();

    // read ini file
    ini_data ini_d = init_readfile();

    // weights and biases
    model_param wb = init_wb(ini_d);
    
    // run commands from init file
    ts_size = ini_d.ts_size; // stores TS size in global variable
    mb_size = ini_d.mb_size; // stores mini-batch size in global variable
    if (ini_d.flag_genTS == true) // generates a new TS and DB for a chosen fx
    {
        xfx_pair *train_set = NULL;
        train_set = generate_TS(ini_d.fx_choice, ini_d.fx_a, \
            ini_d.fx_b, ini_d.fx_c);
        create_db(train_set);
        free_mem(train_set);
    }
    else
    {
        ts_size = count_db();
    }
    eta = ini_d.eta; // learning rate
    epoch_number = ini_d.epoch_num;
    delta = ini_d.delta; // threshold converg

    return wb;
}

// ---------------------------------------------
// generates random seed from time
void init_rseed(void)
{
    // random seed
    srand(time(NULL));
}

// ---------------------------------------------
// initializes weights and biases
model_param init_wb(ini_data ini_d)
{
    // weights and biases
    // if possible generate from Gaussian dist
    float init_w2[3][3] = {0};
    float init_b2[3] = {0};
    float init_w1[3][3] = {0};
    float init_b1[3] = {0};
    
    for (int i = 0; i < 3; i++)
    {
        init_w2[0][i] = ini_d.wl2[i];
        init_w1[i][0] = ini_d.wl1[i];
        init_b1[i] = ini_d.bl1[i];
    }
    init_b2[0] = ini_d.bl2;

    model_param wb;  
    memcpy(wb.w_layer_2, init_w2, sizeof(init_w2)); // third (output) layer
    memcpy(wb.b_layer_2, init_b2, sizeof(init_b2));
    memcpy(wb.w_layer_1, init_w1, sizeof(init_w1)); // second (hidden) layer
    memcpy(wb.b_layer_1, init_b1, sizeof(init_b1));

    return wb;
}

// ---------------------------------------------
// read ini file 
ini_data init_readfile(void)
{
    // read ini file
    const int DATA_LEN = 11; // number of data chars in each line
    const int LINE_LENGTH = 100; 
    char line[LINE_LENGTH]; // total length of line 
    char data[DATA_LEN] = {};
    ini_data ini_d;

    FILE *init_file = fopen(FILE_CONFIG,"r");
    if (init_file == NULL)
    {
        printf("I/O error: ini file.");
        exit(1);
    }  
    
    // read each line
    const int INP_MAX = 20; // number of inputs to read
    int i = 0;
    char inputs[INP_MAX][DATA_LEN] = {}; // contains the INP_MAX inputs
    fgets(line, LINE_LENGTH - 1, init_file); // eliminates first line
    fgets(line, LINE_LENGTH - 1, init_file); // eliminates second line
    while (fgets(line, LINE_LENGTH - 1, init_file) != NULL) 
    {
        // save all input in inputs
        sscanf(line, "%s", data);
        data[DATA_LEN-1] = '\0';
        memcpy(inputs[i], data, sizeof(data));
        i++;
    } 
     
    fclose(init_file);
  
    // loading inputs --> ini_d structure
    if (inputs[0][0] == 'Y') {ini_d.flag_genTS = true;}
    ini_d.ts_size = atoi(inputs[1]);
    ini_d.mb_size = atoi(inputs[2]);
    ini_d.fx_choice = inputs[3][0];
    ini_d.fx_a = atof(inputs[4]); 
    ini_d.fx_b = atof(inputs[5]); 
    ini_d.fx_c = atof(inputs[6]); 
    ini_d.eta = atof(inputs[7]);
    ini_d.epoch_num = atoi(inputs[8]);
    ini_d.delta = atof(inputs[9]);
    for (int i = 0; i < 3; i++)
    {
        ini_d.wl1[i] = atof(inputs[10+i]);
        ini_d.wl2[i] = atof(inputs[13+i]);
        ini_d.bl1[i] = atof(inputs[16+i]);
    }
    ini_d.bl2 = atof(inputs[19]);

    return ini_d;
}


// ---------------------------------------------
// TRAINING SET
// ---------------------------------------------

// ---------------------------------------------
// generates de training set
xfx_pair *generate_TS(char fx_choice, \
        float fx_a, float fx_b, float fx_c)
{
    xfx_pair *train_set = malloc(ts_size * sizeof(xfx_pair));
    if (train_set == NULL)
    {
        printf("Memory allocation failed in generate_TS.\n");
        exit(1);
    }
    
    float rescaled_rand;
    for (int i = 0; i < ts_size; i++)
    {
        // colapse random number to the interval of interest
        rescaled_rand = (float) (rand()-RAND_MAX/2)/(RAND_MAX/2)*X_EXTREME;
        train_set[i].x = rescaled_rand;
        train_set[i].fx = calculate_fx(train_set[i].x, fx_choice, fx_a, \
            fx_b, fx_c);
    } 
    
    return train_set;
}

// ---------------------------------------------
// calcutes f(x) from the chosen function
float calculate_fx(float x, char fx_choice, float fx_a, \
    float fx_b, float fx_c)
{
    float a = fx_a;
    float b = fx_b;
    float c = fx_c;

    switch (fx_choice)
    {
    case 'A':   // a*(x-b)^2+c
        return a*(x-b)*(x-b) + c;
    case 'B':   // a*x+b
       return a*x + b; 
    case 'C':
        return a*cos(b*x) + c;
    default:
        printf("Error in init file: no fx_choice, in \
            function calculate_fx.\n");
        exit(1);
    }
}

// ---------------------------------------------
void free_mem(xfx_pair *ts)
{
    free(ts);
}


// ---------------------------------------------
// SQL
// ---------------------------------------------

// ---------------------------------------------
// creates a db with the training set
int create_db(xfx_pair *train_set)
{
    sqlite3 *db;
    int db_status;
    char *err_msg = 0;

    // open/create db
    db_status = sqlite3_open(DB_NAME, &db);
    if (db_status != SQLITE_OK) 
    {
        fprintf(stderr, "Error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    char sql_qry[256];

    // create table
    strcpy(sql_qry, "CREATE TABLE IF NOT EXISTS xfx (id INTEGER PRIMARY KEY, \
        x REAL, fx REAL);");
    db_status = sqlite3_exec(db, sql_qry, 0, 0, &err_msg);
    if (db_status != SQLITE_OK)
    {
        fprintf(stderr, "Error: %s\n", err_msg);
    }
 
    // delete previous data
    sprintf(sql_qry, "DELETE FROM xfx;");
    db_status = sqlite3_exec(db, sql_qry, callback, 0, &err_msg);
    if (db_status != SQLITE_OK)
    {
        fprintf(stderr, "Error: %s\n", err_msg);
    }

    const int LENGTH_SQLQRY = 60;
    char sql_qry_ins[LENGTH_SQLQRY];
    // Begin transaction
    db_status = sqlite3_exec(db, "BEGIN TRANSACTION;", callback, 0, &err_msg);
    if (db_status != SQLITE_OK)
    {
        fprintf(stderr, "Error: %s\n", err_msg);
        return 1;
    }
    // insert new training set
    for (int i = 0; i < ts_size; i++)
    {
        sprintf(sql_qry_ins, "INSERT INTO xfx (x, fx) VALUES (%7.4f,%7.4f);", \
            train_set[i].x, train_set[i].fx);
        db_status = sqlite3_exec(db, sql_qry_ins, callback, 0, &err_msg);
        if (db_status != SQLITE_OK)
        {
            fprintf(stderr, "Error: %s\n", err_msg);
            break;
        }
    }
    // Commit the transaction if all statements were successful
    db_status = sqlite3_exec(db, "COMMIT;", NULL, 0, &err_msg);
    if (db_status != SQLITE_OK) {
        fprintf(stderr, "Failed to commit transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_exec(db, "ROLLBACK;", NULL, 0, NULL); // Rollback on commit failure
        sqlite3_close(db);
        return 1;
    } 
  
    sqlite3_close(db);

    return 0;
}

// ---------------------------------------------
// reads an existent DB and loads the TS
xfx_pair *read_TS(void)
{
    xfx_pair *train_set = malloc(ts_size*sizeof(xfx_pair));
    if (train_set == NULL)
    {
        printf("Memory allocation failed in read_TS.\n");
        exit(1);
    }

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int db_status;
 
    // open/create db
    db_status = sqlite3_open(DB_NAME, &db);
    if (db_status != SQLITE_OK) 
    {
        fprintf(stderr, "Error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char sql_qry[256] = "SELECT * FROM xfx;";

    // read training set
    // Prepare the statement
    db_status = sqlite3_prepare_v2(db, sql_qry, -1, &stmt, 0);
    if (db_status != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Execute the statement
    int i = 0;
    while ((db_status = sqlite3_step(stmt)) != SQLITE_DONE \
            && i < ts_size)
    {
        train_set[i].x = sqlite3_column_double(stmt, 1);
        train_set[i].fx = sqlite3_column_double(stmt, 2);
        i++;
    }
    
    // Finalize the statement
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    
    return train_set;
}

// ---------------------------------------------
// counts number of elements in DB
int count_db(void)
{
    xfx_pair *train_set = malloc(ts_size*sizeof(xfx_pair));
    if (train_set == NULL)
    {
        printf("Memory allocation failed in read_TS.\n");
        exit(1);
    }

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int db_status;
 
    // open/create db
    db_status = sqlite3_open(DB_NAME, &db);
    if (db_status != SQLITE_OK) 
    {
        fprintf(stderr, "Error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char sql_qry[256] = "SELECT COUNT(*) FROM xfx;";

    // Prepare the statement
    db_status = sqlite3_prepare_v2(db, sql_qry, -1, &stmt, 0);
    if (db_status != SQLITE_OK) 
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Execute the statement
    db_status = sqlite3_step(stmt);
    int cnt = sqlite3_column_int(stmt, 0);

    // Finalize the statement
    sqlite3_finalize(stmt);

    sqlite3_close(db);

    return cnt;
}

// ---------------------------------------------
// auxiliary function to send commands to SQL
int callback(void *NotUsed, int argc, \
    char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


// ---------------------------------------------
// PORPAGATION
// ---------------------------------------------

// ---------------------------------------------
// calculates forward pass
activation forward_pass(model_param wb, float x)
{    
    // each row is a 'layer', upper row = layer 0
    activation a = {
        {x,0,0},
        {0,0,0},
        {0,0,0}
    };
    activation a_new = {
        {x,0,0},
        {0,0,0},
        {0,0,0}
    };
    
    // activations in layer 1 [note eqs. 10-12]
    for (int i = 0; i < 3; i++) // i neuron in layer 1
    {
        for (int j = 0; j < 3; j++) // matrix multiplication w^1.a^0
        {
            a_new.layer_1[i] += wb.w_layer_1[i][j] * a.layer_0[j];
        }
        a_new.layer_1[i] += wb.b_layer_1[i]; // add bias
        a_new.layer_1[i] = sigma_layer_1(a_new.layer_1[i]);
    }
    // activations in layer 2 [note eqs. 9]
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            a_new.layer_2[i] += wb.w_layer_2[i][j] * a_new.layer_1[j];
        }
        a_new.layer_2[i] += wb.b_layer_2[i];
        a_new.layer_2[i] = sigma_layer_2(a_new.layer_2[i]);
    }

    return a_new;
}

// ---------------------------------------------
// calculates forward pass: only z
activation forward_pass_z(model_param wb, float x)
{
    activation a = {
        {x,0,0},
        {0,0,0},
        {0,0,0}
    };
    activation a_new = {
        {0,0,0},
        {0,0,0},
        {0,0,0}
    };
    activation z = {0};
    
    // activations in layer 1 [note eqs. 10-12]
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // part of z
            a_new.layer_1[i] += wb.w_layer_1[i][j] * a.layer_0[j];
        }
        z.layer_1[i] = a_new.layer_1[i] + wb.b_layer_1[i]; // z_i^1
        a_new.layer_1[i] = sigma_layer_1(z.layer_1[i]);
    }
    // activations in layer 2 [note eqs. 9]
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            a_new.layer_2[i] += wb.w_layer_2[i][j] * a_new.layer_1[j];
        }
        z.layer_2[i] = a_new.layer_2[i] + wb.b_layer_2[i]; // z_i^2
    }

    return z;
}

// ---------------------------------------------
// activation function
float sigma_layer_1(float a)
{
    // ReLU
    if (a > 0) {a = a;}
    else {a = 0;}
    /* Tanh
    a = tanh(a);
    */

    return a;
}

// ---------------------------------------------
// activation function
float sigma_layer_2(float a)
{
    // linear
    return a;
}

// ---------------------------------------------
// derivative of activation function
float deriv_sigma_layer_1(float a)
{
    // ReLU
    if (a > 0) {a = 1;}
    else {a = 0;}

    /* Tanh
    a = 1 - tanh(a)*tanh(a);
    */

    return a;
}

// ---------------------------------------------
// derivative of activation function
float deriv_sigma_layer_2(float a)
{
    return 1;
}

// ---------------------------------------------
// shuffles TS for the use in each epoch
xfx_pair *shuffle_TS(xfx_pair *train_set)
{
    // can change algorithm to Fisher–Yates
    xfx_pair *shuffled_ts = malloc(ts_size*sizeof(xfx_pair));
    if (shuffled_ts == NULL)
    {
        printf("Error allocating memory in shuffle_TS\n");
        exit(1);
    }
    bool *used_slots = calloc(ts_size, sizeof(bool)); // init to zero=false
    if (used_slots == NULL) 
    {
        printf("Error allocating memory in shuffle_TS\n");
        exit(1);
    }
    int rand_slot = 0;
    
    for (int i = 0; i < ts_size; i++)
    {
        rand_slot = rand() % (ts_size);        
        if (used_slots[rand_slot] == false)
        {
            shuffled_ts[rand_slot].x = train_set[i].x;
            shuffled_ts[rand_slot].fx = train_set[i].fx;
            used_slots[rand_slot] = true;
        }
        else {i--;}
    } 

    free(used_slots);
    return shuffled_ts;
}

// ---------------------------------------------
// creates a minibatch mb_index from shuffled TS
xfx_pair *isolate_mb(xfx_pair *shuffled_ts, int mb_index)
{
    xfx_pair *mb = calloc(mb_size, sizeof(xfx_pair));

    memcpy(mb, (shuffled_ts + mb_index*mb_size), \
        mb_size*sizeof(xfx_pair));

    return mb;
}

// ---------------------------------------------
// minibatch calculation of C and GradC
loss_gradloss calculate_CgradC(xfx_pair *mb, model_param wb)
{
    // calculate_CGradC
    
    loss_gradloss CgradC_mb = {0};
    float error_layer_1[3] = {0};
    float error_layer_2[1] = {0}; 

    // Loop for calculating all x in mini-batch
    for (int i = 0; i < mb_size; i++)
    {
        loss_gradloss CgradC_x = {0};
        // forward pass
        activation a = forward_pass(wb, mb[i].x);
        // backward pass
        activation z = forward_pass_z(wb, mb[i].x);
        error_layer_2[0] = (a.layer_2[0] - \
            mb[i].fx) * deriv_sigma_layer_2(z.layer_2[0]);
        for (int j = 0; j < 3; j++) 
        {
            error_layer_1[j] = error_layer_2[0] * wb.w_layer_2[0][j] * \
                deriv_sigma_layer_1(z.layer_1[j]);
        }
        // CgradC.gradC
        for (int k = 0; k < 3; k++)
        {
        CgradC_x.gradC[k] = error_layer_2[0] * a.layer_1[k];
        CgradC_x.gradC[k+3] = error_layer_1[k] * a.layer_0[0];
        CgradC_x.gradC[k+7] = error_layer_1[k];
        }
        CgradC_x.gradC[6] = error_layer_2[0];

        // CgradC.C - Loss function
        CgradC_x.C = (1/2.) * pow(a.layer_2[0] - mb[i].fx, 2);

        // cumulatives
        CgradC_mb.C += CgradC_x.C;
        for (int k = 0; k < 10; k++)
            {CgradC_mb.gradC[k] += CgradC_x.gradC[k];}
    }
    CgradC_mb.C = CgradC_mb.C / mb_size;
    for (int k = 0; k < 10; k++)
        {CgradC_mb.gradC[k] = CgradC_mb.gradC[k] / mb_size;}

    return CgradC_mb;
}


