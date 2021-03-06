#include <stdio.h>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include "image.h"
#include "read_images.h"
#include "extract_feature.h"
#include "kmeans.h"
#include "debug.h"
#include "build_vocab_pipeline.h"
using namespace std;

// sample from all local descriptors
void get_sample_local_desc(vector<local_desc_type> &all_local_desc, int sample_count)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(all_local_desc.begin(), all_local_desc.end(), default_random_engine(seed));
    if (sample_count < all_local_desc.size())
    all_local_desc.resize(sample_count);
}

int main(int argc, char *argv[])
{
	if(argc < 4)
    {
        printf("Usage: build_vocab_pipeline [Png Dir Name] [Sample Count] [Vocab File Name] [Num of Pic to Use]\n");
        return 1;
    }
    char *png_dir = argv[1], *vocab_fname = argv[3];
    int sample_count = atoi(argv[2]), vocab_size = NUM_VOCAB;
    vector<local_desc_type> all_local_desc;

    vector<string> png_file_list = read_png_dir(png_dir);
    if (png_file_list.size() == 0)
    {
        printf("Error: invalid png dir name\n");
        return 1;
    }
    int png_file_count = png_file_list.size();
    if (argc >= 5)
        png_file_count = atoi(argv[4]);

    for (int i = 0;i < png_file_count;++i)
    {
        image_type image = png_to_image(png_file_list[i].c_str());
        vector<local_desc_type> feature_descs = extract_features_pipeline(image);
        all_local_desc.insert(all_local_desc.end(), feature_descs.begin(), feature_descs.end());
        printf("extraction progress: %d finished\n", i + 1);
    }
    get_sample_local_desc(all_local_desc, sample_count);
    // print_vec(all_local_desc);
    vector<local_desc_type> init_centers = kmeans_plusplus(all_local_desc, vocab_size);
    vector<local_desc_type> centers = kmeans(all_local_desc, init_centers, vocab_size);
    save_vocab(centers, vocab_fname);
    
    return 0;
}
