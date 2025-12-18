/*
CODE
Header for the nnfit project
*/

#ifndef NN_H
#define NN_H

#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "sqlite3.h"

// structures
typedef struct 
{
    float x;
    float fx;
} xfx_pair; // {x, f(x)} pair
typedef struct 
{
    bool flag_genTS;
    int ts_size;
    int mb_size;
    char fx_choice;
    float fx_a;
    float fx_b;
    float fx_c;
    double eta;
    int epoch_num;
    double delta;
    float wl1[3];
    float wl2[3];
    float bl1[3];
    float bl2;
} ini_data; // translated data from ini file
typedef struct 
{
    float layer_0[3];
    float layer_1[3];
    float layer_2[3];
} activation; // activations
typedef struct 
{
    float w_layer_1[3][3];
    float w_layer_2[3][3];
    float b_layer_1[3];
    float b_layer_2[3];
} model_param; // weights and biases
typedef struct 
{
    float C;
    float gradC[10]; // see notes eqs. 27-36
} loss_gradloss; // weights and biases


// global variables from ini file
extern int ts_size;
extern int mb_size;
extern double eta; // learning rate
extern int epoch_number;
extern double delta; // threshold converg


// prototypes
// -- TS
xfx_pair *generate_TS(char fx_choice, \
        float fx_a, float fx_b, float fx_c);
float calculate_fx(float x, char fx_choice, \
    float fx_a, float fx_b, float fx_c);
void free_mem(xfx_pair *ts);
// -- init
model_param init(void);
void init_rseed(void);
model_param init_wb(ini_data ini_d);
ini_data init_readfile(void);
// -- SQL
int callback(void *NotUsed, int argc, char **argv, \
    char **azColName);
int create_db(xfx_pair *ts);
xfx_pair *read_TS(void);
int count_db(void);
// -- forward and backward passes
activation forward_pass(model_param wb, float x);
activation forward_pass_z(model_param wb, float x);
float sigma_layer_1(float a);
float deriv_sigma_layer_1(float a);
float sigma_layer_2(float a);
float deriv_sigma_layer_2(float a);
xfx_pair *shuffle_TS(xfx_pair *ts);
loss_gradloss calculate_CgradC(xfx_pair *mb, \
    model_param wb);
xfx_pair *isolate_mb(xfx_pair *shuffled_ts, \
    int mb_index);

#endif