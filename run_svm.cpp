#include <stdio.h>
#include <stdlib.h>
#include "svm.h"

int main(int argc, char *argv[])
{
    // train set size : test set size = 2 : 1
    if (argc <= 2)
    {
        printf("Usage: run_svm [Feature File Path] [Num Image Per Category]\n");
        return 1;
    }
    char *feature_file_path = argv[1];
    int image_per_category = atoi(argv[2]);
    gen_svm_data(feature_file_path, image_per_category);    
    svm_train_wrapper(svm_gamma, svm_cost);

    return 0;
}
