#include <gtkmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "image.h"
#include "read_images.h"
#include "extract_feature.h"
#include "build_vocab_pipeline.h"
#include "svm.h"
#include "libsvm.h"
using namespace std;

struct svm_model* model;

int predict(image_hist_type sketch_hist)
{
    int n = sketch_hist.NR;
    struct svm_node *x = (struct svm_node *) malloc(sizeof(struct svm_node) * (n + 10));
    for (int i = 0;i < n;++i)
    {
        x[i].index = i + 1;
        x[i].value = sketch_hist(i);
    }
    x[n].index = -1;
    double predict_label = svm_predict(model, x);
    return int(predict_label + 1e-7);
}

int classify_image(char *image_file, vector<local_desc_type> vocab)
{
    image_type image = png_to_image(image_file);
    vector<local_desc_type> feature_descs = extract_features_pipeline(image);
    image_hist_type sketch_hist;
    build_sketch_hist(feature_descs, vocab, sketch_hist);
    // cout << sketch_hist << endl;
    return predict(sketch_hist);
}

int main(int argc, char *argv[])
{
    // Glib::RefPtr<Gtk::Application> app =
    //     Gtk::Application::create(argc, argv,
    //       "org.gtkmm.examples.base");
    // 
    // Gtk::Window window;
    // window.set_default_size(200, 200);

    // return app->run(window);
    if (argc < 5)
    {
        printf("Usage: app [Image File] [Vocab File] [SVM Model File] [Label Map File]\n");
        return 1;
    }
    char *image_file = argv[1], *vocab_file = argv[2], *model_file = argv[3], *label_map_file = argv[4];
    char new_image_file[1000];
    vector<local_desc_type> vocab;
    load_vocab(vocab_file, vocab);
    model = svm_load_model(model_file);
    if (model == NULL)
    {
        printf("Error: invalid model file\n");
        return 1;
    }

    while (1)
    {
        printf("prompt for input:\n");
        scanf("%s", new_image_file);
        image_file = new_image_file;
        int label = classify_image(image_file, vocab);
        printf("label: %d\n", label);
    }
    return 0;
}
