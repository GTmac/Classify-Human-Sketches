#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <dlib/matrix.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include "image.h"

using namespace std;

const int MAX_STRLEN = 1111;
int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

void read_png_file(const char *filename) {
	FILE *fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if (!png)
        return;
	if (!info)
        return;
	if(setjmp(png_jmpbuf(png) ) )
        return;

	png_init_io(png, fp);
	png_read_info(png, info);

	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth = png_get_bit_depth(png, info);

	if(bit_depth == 16)
	    png_set_strip_16(png);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	if(color_type == PNG_COLOR_TYPE_RGB ||
		 color_type == PNG_COLOR_TYPE_GRAY ||
		 color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if(color_type == PNG_COLOR_TYPE_GRAY ||
		 color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int y = 0; y < height; y++)
	    row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png,info) );
	png_read_image(png, row_pointers);
	fclose(fp);
}

image_type png_to_image(const char *fname) {
    image_type image;
    read_png_file(fname);
    // printf("height and width: %d %d\n", height, width);
    for(int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            // convert to matrix
            image(y, x) = (.299 * px[0] + .587 * px[1] + .114 * px[2]) / 255.0;
        }
    }
    image = 1.0 - image;
    //for (int y = 0;y < height;++y)
    //{
    //    for (int x = 0;x < width;++x)
    //        printf("%.3lf\n", image(y, x));
    //}
    return image;
}

// iterate through the whole png dir, no thumbnail allowed
vector<string> read_png_dir(char *dir_name)
{
    DIR *dir = opendir(dir_name);
    struct dirent* entry;
    vector<string> png_list;

    if (!dir)
        return png_list;

    while ( (entry = readdir(dir)))
    {
        // to the level of each category
        if (entry -> d_type == DT_DIR)
        {
            char png_file_path[MAX_STRLEN], category_path[MAX_STRLEN] = "";
            if (!strcmp(entry -> d_name, "..") || !strcmp(entry -> d_name, "."))
                continue;
            sprintf(category_path, "%s/%s", dir_name, entry -> d_name);
            DIR *category_dir = opendir(category_path);
            struct dirent* category_entry;
            while ( (category_entry = readdir(category_dir)) )
            {
                // not thumbnail
                if (category_entry -> d_type == DT_REG && category_entry -> d_name[0] != '.' && !strstr(category_entry -> d_name, "thumbnail"))
                {
                    sprintf(png_file_path, "%s/%s", category_path, category_entry -> d_name);
                    png_list.push_back(string(png_file_path));
                }
            }
        }
    }
    return png_list;
}

// int main(int argc, char *argv[])
// {
//     if (argc != 2)
//     {
//         printf("Usage: read_images [Png Dir Name]\n");
//         return 1;
//     }
//     vector<string> s=read_png_dir(argv[1]);
//     for (auto iter: s)
//         cout << iter << endl;
// }
