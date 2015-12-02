#include <vector>
const double MAX_DIS = 1e9;
const int KMEANS_ITER = 1000;
std::vector<local_desc_type> kmeans_plusplus(std::vector<local_desc_type> local_desc_list, int k);
std::vector<local_desc_type> kmeans(std::vector<local_desc_type> local_desc_list, std::vector<local_desc_type> init_centers, int k);
