// read from the features file for all images, and build dataset of different set.
// generate 3 datasets: full dataset with all feature hists, train dataset and test dataset
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include "svm.h"
using namespace std;

void gen_svm_data(char *feature_file_path, int image_per_category)
{
    char line[MAX_LEN], str_to_print[MAX_LEN];
    vector<vector<float> > image_feature_list;
    FILE* fp = fopen(feature_file_path, "r");
    if (fp == NULL)
    {
        printf("Error: Invalid feature file path\n");
        fclose(fp);
        return;
    }
    int sample_count = 0;
    while (fgets(line, MAX_LEN, fp) != NULL)
    {
        vector<float> image_feature;
        // read a line of feature, which is for a single image
        int offset = 0;
        float val;
        char *line_ptr = line;
        int category_label = sample_count / IMAGE_PER_CATEGORY;

        // sample from each category
        if (sample_count % IMAGE_PER_CATEGORY < image_per_category)
        {
            // add category label
            image_feature.push_back(float(category_label));
            while (sscanf(line_ptr, "%f%n", &val, &offset) == 1)
            {
                image_feature.push_back(val);
                line_ptr += offset;
            }
            image_feature_list.push_back(image_feature);
        }
        ++sample_count;
    }
    fclose(fp);
    //printf("image feature list size: %lu\n", image_feature_list.size());

    // shuffle all data we have
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(image_feature_list.begin(), image_feature_list.end(), default_random_engine(seed));
    // build LibSVM format datasets
    int feature_list_size = image_feature_list.size(), train_dataset_size = 2 * feature_list_size / 3;
    FILE *all_svm_data_fp = fopen(all_svm_data_fname, "w");
    FILE *train_svm_data_fp = fopen(train_svm_data_fname, "w");
    FILE *test_svm_data_fp = fopen(test_svm_data_fname, "w");

    for (int i = 0;i < feature_list_size;++i)
    {
        int feature_size = image_feature_list[i].size();
        for (int j = 0;j < feature_size;++j)
        {
            if (!j)
                sprintf(str_to_print, "%d", int(image_feature_list[i][j] + 1e-7));
            else
                sprintf(str_to_print, " %d:%.6f", j, image_feature_list[i][j]);
            fprintf(all_svm_data_fp, "%s", str_to_print);
            if (i < train_dataset_size)
                fprintf(train_svm_data_fp, "%s", str_to_print);
            else
                fprintf(test_svm_data_fp, "%s", str_to_print);
        }
        fprintf(all_svm_data_fp, "\n");
        if (i < train_dataset_size)
            fprintf(train_svm_data_fp, "\n");
        else
            fprintf(test_svm_data_fp, "\n");
    }
    fclose(all_svm_data_fp);
    fclose(train_svm_data_fp);
    fclose(test_svm_data_fp);
}

// return accuracy
float svm_train_wrapper(float svm_gamma, float svm_cost)
{
    char train_svm_cmd[MAX_LEN], result[MAX_LEN];
    sprintf(train_svm_cmd, "./svm-train -g %f -c %f -v 3 -s 1 -q %s", svm_gamma, svm_cost, all_svm_data_fname);
    printf("Training SVM with command: %s\n", train_svm_cmd);
    FILE* fp = popen(train_svm_cmd, "r");
    if (fp == NULL)
    {
        printf("Fail to run svm-train.\n");
        return 0.0;
    }
    fgets(result, MAX_LEN, fp);
    // remove %
    result[strlen(result) - 1] = '\0';
    int acc_start_index;
    for (acc_start_index = strlen(result) - 1;acc_start_index >= 0 && result[acc_start_index] != '=';--acc_start_index);
    float accuracy = atof(result + acc_start_index + 1);
    printf("SVM classification accuracy: %.2f\n", accuracy);
    return accuracy;
}
