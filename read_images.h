#ifndef _READ_IMAGES_H
#define _READ_IMAGES_H
#include "image.h"
#include <vector>
image_type png_to_image(const char *fname);
std::vector<std::string> read_png_dir(char *dir_name);
#endif
