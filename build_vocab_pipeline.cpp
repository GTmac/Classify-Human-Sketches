#include <stdio.h>
#include <vector>
#include "image.h"
#include "read_images.h"
#include "extract_feature.h"
using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 2)
    {
        printf("Usage: build_vocab_pipeline [File Name]\n");
        return 1;
    }
    image_type image = png_to_image(argv[1]); 
    vector<local_desc_type> feature_descs = extract_features_pipeline(image);
    return 0;
}
