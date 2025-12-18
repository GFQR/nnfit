/*
TEST CODE
main code for the nnfit project
*/

#include "nnfit.h"


int main(void)
{
    // initialization
    model_param wb = init(); // wb is weights and biases


    // load training set from DB
    xfx_pair *train_set = NULL;
    train_set = read_TS();


    // ---------
    // MAIN LOOP
    xfx_pair *shuffled_ts = NULL;
    xfx_pair *mb = NULL;
    float *C_epoch = calloc(epoch_number, sizeof(float));
    float *eta_epoch = \
        calloc(epoch_number, sizeof(float)); // for printing
    int mb_number = (int) ts_size/mb_size;
    loss_gradloss CgradC_mb;
    int epoch_converged = 0;

    // loop all epochs
    for (int epoch_index = 0; epoch_index < epoch_number; epoch_index++)
    {
        // shuffle TS for each epoch
        shuffled_ts = shuffle_TS(train_set);


        // loop all minibatches from 0...mb_number
        for (int mb_index = 0; mb_index < mb_number; mb_index++)
        {
            CgradC_mb = (loss_gradloss){0}; // compound literal
            mb = isolate_mb(shuffled_ts, mb_index);
            CgradC_mb = calculate_CgradC(mb, wb); // single mb calculation
            // update of wb.w_layer_2
            for(int i = 0; i < 3; i++)
                {wb.w_layer_2[0][i] -= eta * CgradC_mb.gradC[i];}
            // update of wb.w_layer_1
            for(int i = 0; i < 3; i++)
                {wb.w_layer_1[i][0] -= eta * CgradC_mb.gradC[i+3];}
            // wb.b_layer_2
            wb.b_layer_2[0] -= eta * CgradC_mb.gradC[6];
            // wb.b_layer_1
            for(int i = 0; i < 3; i++)
                {wb.b_layer_1[i] -= eta * CgradC_mb.gradC[i+7];}
            // accumulating C from all mb
            C_epoch[epoch_index] += CgradC_mb.C;

            free(mb);
            mb = NULL;
        }
        C_epoch[epoch_index] = C_epoch[epoch_index] / (mb_number); // average
        // check, every epoch_chk_freq if C converged
        if ((epoch_index % 5) == 0 && C_epoch[epoch_index] < delta)
        {
            break;
        }
        epoch_converged = epoch_index;

        // auto-adjust eta for faster convergence
        float eta_adjustment = 0.8;
        if (epoch_index > 0)
        {
            if ( fabs( 2* (C_epoch[epoch_index-1] - C_epoch[epoch_index]) \
                / (C_epoch[epoch_index-1] + C_epoch[epoch_index]) ) < eta) \
                {eta *= eta_adjustment;}
        }

        eta_epoch[epoch_index] = eta;

        free(shuffled_ts);
        shuffled_ts = NULL;
    }


    // print to stdout in JSON format
    printf("{\n");

    printf("  \"weights\": {\n");
    printf("    \"w_layer_1\": [");
    printf("%7.5f, ", wb.w_layer_1[0][0]);
    printf("%7.5f, ", wb.w_layer_1[1][0]);
    printf("%7.5f",  wb.w_layer_1[2][0]);
    printf("],\n");

    printf("    \"b_layer_1\": [");
    printf("%7.5f, ", wb.b_layer_1[0]);
    printf("%7.5f, ", wb.b_layer_1[1]);
    printf("%7.5f",  wb.b_layer_1[2]);
    printf("],\n");

    printf("    \"w_layer_2\": [");
    printf("%f, ", wb.w_layer_2[0][0]);
    printf("%f, ", wb.w_layer_2[0][1]);
    printf("%f",  wb.w_layer_2[0][2]);
    printf("],\n");

    printf("    \"b_layer_2\": %f\n", wb.b_layer_2[0]);
    printf("  },\n");

    printf("  \"loss\": {\n");
    for (int i = 0; i < epoch_converged; i++) {
        printf("    \"%d\": %f", i, C_epoch[i]);
        if (i < epoch_converged - 1) printf(",");
        printf("\n");
    }
    printf("  }\n");

    printf("}\n");


    // free memory
    free(train_set);
    free(shuffled_ts);
    free(mb);
    free(C_epoch);
    free(eta_epoch);

    return 0;
 }
