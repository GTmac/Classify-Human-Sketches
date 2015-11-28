#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <dlib/matrix.h>
#include "image.h"
#define IMAGE_SIZE  1111

using namespace dlib;

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

void read_png_file(char *filename) {
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

image_type png_to_image(char *fname) {
    read_png_file(fname);
    image_type image;
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
