#ifndef _IMAGE_H
#define _IMAGE_H
const int NUM_ORIENT_BINS = 4;
const int NUM_SPATIAL_BINS = 4;
const int NUM_VOCAB = 500;
const double SPATIAL_BIN_COVER = 0.125;
const double my_eps = 1e-7;
const int LOCAL_DESC_SIZE = NUM_ORIENT_BINS * NUM_SPATIAL_BINS * NUM_SPATIAL_BINS;
const int IMAGE_SIZE = 1111, HALF_IMAGE_SIZE = IMAGE_SIZE / 2 + 1;

#include <dlib/matrix.h>
typedef dlib::matrix<float, IMAGE_SIZE, IMAGE_SIZE> image_type;
typedef dlib::matrix<std::complex<float>, IMAGE_SIZE, IMAGE_SIZE / 2 + 1> complex_image_type;
typedef dlib::matrix<float, LOCAL_DESC_SIZE, 1> local_desc_type;
typedef dlib::matrix<float, LOCAL_DESC_SIZE, 1> vocab_type;
#endif
