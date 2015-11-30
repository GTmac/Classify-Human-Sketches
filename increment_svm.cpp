#include <stdio.h>
#include "svm.h"

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("Usage: increment_svm [Feature File Path]\n");
        return 1;
    }
    char *feature_file_path = argv[1];
    for (int image_per_category = 8;image_per_category <= 80;image_per_category += 8)
    {
        printf("Image count per category: %d\n", image_per_category);
        gen_svm_data(feature_file_path, image_per_category);
        svm_train_wrapper(svm_gamma, svm_cost);
    }
    return 0;
}
