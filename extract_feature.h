#ifndef _EXTRACT_FEATURE_H
#define _EXTRACT_FEATURE_H
#include "image.h"
#include <vector>
std::vector<local_desc_type> extract_features_pipeline(image_type image);
std::vector<image_type> get_gaussian_gradient(image_type image);
std::vector<local_desc_type> extract_desc(std::vector<image_type> image_hist);
void build_sketch_hist(std::vector<local_desc_type> feature_descs, std::vector<local_desc_type> vocab, image_hist_type &sketch_hist);
#endif

