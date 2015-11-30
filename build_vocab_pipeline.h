#ifndef _VOCAB_H
#define _VOCAB_H
#include "image.h"
#include <vector>
void save_vocab(std::vector<local_desc_type> vocab, char *vocab_fname);
void load_vocab(char *vocab_fname, std::vector<local_desc_type> &vocab);
#endif
