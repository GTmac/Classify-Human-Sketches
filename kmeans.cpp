#include <random>
#include <vector>
#include <dlib/matrix.h>
#include <time.h>
#include "image.h"
#include "kmeans.h"
#include "debug.h"
using namespace std;
const double MAX_DIS = 1e9;
const double KMEANS_ITER = 200;

// K-means++ for initialize the cluster centers
vector<local_desc_type> kmeans_plusplus(vector<local_desc_type> local_desc_list, int k)
{
    int n = local_desc_list.size();
    default_random_engine generator;
    vector<local_desc_type> centers;
    vector<float> dis_to_center(n, MAX_DIS);

    srand(time(NULL));

    int init_index = rand() % n;
    centers.push_back(local_desc_list[init_index]);
    // init distances
    for (int i = 0;i < n;++i)
        dis_to_center[i] = length_squared(centers[0] - local_desc_list[i]);
    
    for (int i = 0;i < k - 1;++i)
    {
        // weighted distribution
        discrete_distribution<float> distribution(dis_to_center.begin(),
                dis_to_center.end());
        int new_center_index = distribution(generator);
        centers.push_back(local_desc_list[new_center_index]);
        // update min distance to center
        for (int j = 0;j < n;++j)
        {
            float new_dist = dlib::length_squared(local_desc_list[new_center_index] - local_desc_list[i]);
            dis_to_center[i] = min(dis_to_center[i], new_dist);
        }
    }
    return centers;
}

// cluster local descriptors into K categories
vector<local_desc_type> kmeans(vector<local_desc_type> local_desc_list, vector<local_desc_type> init_centers, int k)
{
    vector<local_desc_type> centers(init_centers.begin(), init_centers.end());
    local_desc_type zeros;
    int n = local_desc_list.size();
    vector<int> cluster_id(n, 0);
    bool cluster_change = false;
    
    for (int iter_count = 0;iter_count < KMEANS_ITER;++iter_count)
    {
        vector<int> cluster_size(k, 0);
        // assign cluster id for each node
        for (int i = 0;i < n;++i)
        {
            int new_cluster_id = 0;
            float min_dis = MAX_DIS;
            for (int j = 0;j < k;++j)
            {
                float new_dis = dlib::length_squared(centers[j] - local_desc_list[i]);
                // printf("j: %d, new_dis: %f\n", j, new_dis);
                if (new_dis < min_dis)
                {
                    min_dis = new_dis;
                    new_cluster_id = j;
                }
            }
            if (new_cluster_id != cluster_id[i])
            {
                cluster_change = true;
                cluster_id[i] = new_cluster_id;
            }
            ++cluster_size[ cluster_id[i] ];
        }
        // stable clusters
        if (cluster_change == false)
            break;
        // update clusters
        centers.assign(k, dlib::zeros_matrix(zeros));
        for (int i = 0;i < n;++i)
        {
            int cur_cluster_id = cluster_id[i];
            centers[cur_cluster_id] += local_desc_list[i];
        }
        for (int j = 0;j < k;++j)
            if (cluster_size[j] != 0)
                centers[j] /= cluster_size[j]; 
        // verbose output
        printf("K-means round %d:\n", iter_count + 1);
        for (int j = 0;j < k;++j)
            printf("cluster %d size: %d\n", j, cluster_size[j]);
        cluster_change = false;
    }
    return centers;
}
