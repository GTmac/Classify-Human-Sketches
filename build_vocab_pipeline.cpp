#include <stdio.h>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include "image.h"
#include "read_images.h"
#include "extract_feature.h"
#include "kmeans.h"
using namespace std;

// sample from all local descriptors
void get_sample_local_desc(vector<local_desc_type> &all_local_desc, int sample_count)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(all_local_desc.begin(), all_local_desc.end(), default_random_engine(seed));
    if (sample_count < all_local_desc.size())
    all_local_desc.resize(sample_count);
}

void save_vocab(vector<local_desc_type> vocab, char *vocab_fname)
{
    FILE* fp = fopen(vocab_fname, "w");
    int n = vocab.size();
    for (int i = 0;i < n;++i)
    {
        for (int j = 0;j < vocab[i].NR;++j)
        {
            if (j)
                fprintf(fp, " ");
            fprintf(fp, "%.3lf", vocab[i](j, 0));
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
	if(argc < 4)
    {
        printf("Usage: build_vocab_pipeline [Png Dir Name] [Sample Count] [Vocab File Name] (Vocab Size)\n");
        return 1;
    }
    char *png_dir = argv[1], *vocab_fname = argv[3];
    int sample_count = atoi(argv[2]), vocab_size;
    vector<local_desc_type> all_local_desc;
    if (argc >= 5)
        vocab_size = atoi(argv[4]);
    else
        vocab_size = NUM_VOCAB;

    vector<string> png_file_list = read_png_dir(png_dir);
    if (png_file_list.size() == 0)
    {
        printf("Error: invalid png dir name\n");
        return 1;
    }
    int png_file_count = png_file_list.size();

    for (int i = 0;i < 2;++i)
    // for (int i = 0;i < png_file_count;++i)
    {
        image_type image = png_to_image(png_file_list[i].c_str());
        vector<local_desc_type> feature_descs = extract_features_pipeline(image);
        all_local_desc.insert(all_local_desc.end(), feature_descs.begin(), feature_descs.end());
    }
    get_sample_local_desc(all_local_desc, sample_count);
    //for (int i = 0;i < all_local_desc.size();++i)
    //    cout << all_local_desc[i] << endl;
    vector<local_desc_type> init_centers = kmeans_plusplus(all_local_desc, vocab_size);
    vector<local_desc_type> centers = kmeans(all_local_desc, init_centers, vocab_size);
    save_vocab(centers, vocab_fname);
    
    return 0;
}
