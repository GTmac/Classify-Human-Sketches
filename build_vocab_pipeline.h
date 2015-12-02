#ifndef _VOCAB_H
#define _VOCAB_H
#include "image.h"
#include "constant.h"
#include <vector>
using namespace std;

void load_vocab(char *vocab_fname, vector<local_desc_type> &vocab)
{
    char line[MAX_LEN];
    FILE* fp = fopen(vocab_fname, "r");
    if (fp == NULL)
    {
        printf("Error: Invalid vocab file path\n");
        return;
    }
    while (fgets(line, MAX_LEN, fp) != NULL)
    {
        local_desc_type local_desc;
        int offset = 0, index = 0;
        float val;
        char *line_ptr = line;
        while (sscanf(line_ptr, "%f%n", &val, &offset) == 1)
        {
            local_desc(index) = val;
            line_ptr += offset;
            ++index;
        }
        vocab.push_back(local_desc);
    }
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
#endif
