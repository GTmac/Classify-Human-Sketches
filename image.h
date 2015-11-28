#ifndef _IMAGE_H
#define _IMAGE_H
#define NUM_ORIENT_BINS     4
#define NUM_SPATIAL_BINS    4
#define SPATIAL_BIN_COVER   0.125
#define my_eps                 1e-7
const int IMAGE_SIZE = 1111, HALF_IMAGE_SIZE = IMAGE_SIZE / 2 + 1;

#include <dlib/matrix.h>
typedef dlib::matrix<float, IMAGE_SIZE, IMAGE_SIZE> image_type;
typedef dlib::matrix<std::complex<float>, IMAGE_SIZE, IMAGE_SIZE / 2 + 1> complex_image_type;
typedef dlib::matrix<float, NUM_ORIENT_BINS * NUM_SPATIAL_BINS * NUM_SPATIAL_BINS, 1> local_desc_type;
#endif
