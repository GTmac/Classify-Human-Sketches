// generate and feature hists for all images
#include <stdio.h>
#include <vector>
#include "image.h"
#include "read_images.h"
#include "extract_feature.h"
#include "build_vocab_pipeline.h"
#include "constant.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: gen_feature_hists [Png Directory] [Vocab File Name] [Feature Hist File Name] [Png File Count]\n");
        return 1;
    }
    char *png_dir = argv[1], *vocab_fname = argv[2], *feature_hist_fname = argv[3];
    int png_file_count = atoi(argv[4]);
    FILE *fp = fopen(feature_hist_fname, "w");
    vector<local_desc_type> vocab;

    load_vocab(vocab_fname, vocab);
    vector<string> png_file_list = read_png_dir(png_dir);
    int file_count = 0;
    for (auto png_file: png_file_list)
    {
        ++file_count;
        printf("file count: %d\n", file_count);
        image_type image = png_to_image(png_file.c_str());
        vector<local_desc_type> feature_descs = extract_features_pipeline(image);
        image_hist_type sketch_hist;
        build_sketch_hist(feature_descs, vocab, sketch_hist);
        // save the feature hist
        for (int i = 0;i < sketch_hist.NR;++i)
        {
            if (i)
                fprintf(fp, " ");
            fprintf(fp, "%.6f", sketch_hist(i));
        }
        fprintf(fp, "\n");
        if (file_count >= png_file_count)
            break;
    }
    fclose(fp);
    return 0;
}
