#ifndef _SVM_H
#define _SVM_H
const int IMAGE_PER_CATEGORY = 80;
const int CATEGORY_COUNT = 250;
const int IMAGE_COUNT = IMAGE_PER_CATEGORY * CATEGORY_COUNT;
const int MAX_LEN = 100000;
const double svm_gamma = 17.8, svm_cost = 3.2;
#define all_svm_data_fname "../data/all-svm-data"
#define train_svm_data_fname "../data/train-svm-data"
#define test_svm_data_fname "../data/test-svm-data"

float svm_train_wrapper(float svm_gamma, float svm_cost);
void gen_svm_data(char *feature_file_path, int image_per_category);
#endif
