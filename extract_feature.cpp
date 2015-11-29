#include <iostream>
#include <stdio.h>
#include <math.h>
#include <dlib/matrix.h>
#include <fftw3.h>
#include <vector>
#include "debug.h"
#include "image.h"

using namespace std;

int spatial_bin_size = sqrt(SPATIAL_BIN_COVER) * IMAGE_SIZE / NUM_SPATIAL_BINS;
image_type fft_input;
complex_image_type fft_output;
// creating plan is time-consuming, so it's done for only once
fftwf_plan plan = fftwf_plan_dft_r2c_2d(IMAGE_SIZE, IMAGE_SIZE, &fft_input(0, 0), reinterpret_cast<float(*)[2]>(&fft_output(0, 0)), FFTW_ESTIMATE);
fftwf_plan inv_plan = fftwf_plan_dft_c2r_2d(IMAGE_SIZE, IMAGE_SIZE, reinterpret_cast<float(*)[2]>(&fft_output(0, 0)), &fft_input(0, 0), FFTW_ESTIMATE);
complex_image_type complex_tent;

// build tent function, which will be appied to the response image
// a fast way for build the histogram
image_type build_tent()
{
    image_type image_tent;
    // to left and right
    int tent_length = 2 * spatial_bin_size + 1;
    
    for (int i = 0;i < tent_length;++i)
    {
        int dis_x = spatial_bin_size - abs(i - spatial_bin_size);
        for (int j = 0;j < tent_length;++j)
        {
            int dis_y = spatial_bin_size - abs(j - spatial_bin_size);
            image_tent(i, j) = dis_x * dis_y;
        }
    }
    fftwf_execute_dft_r2c(plan, (float*)(&image_tent(0, 0)), reinterpret_cast<float(*)[2]>(&complex_tent(0, 0)));
    complex_tent /= IMAGE_SIZE * IMAGE_SIZE;
    return image_tent;
}

// also init complex tent
image_type image_tent = build_tent();

int get_mod(int x, int k)
{
    return (x % k + k) % k;
}
// based on the gradients we get from Gaussin filters / Gabor filters,
// calculate the reponse images
void get_response_images(image_type l2_len, image_type orientation, vector<image_type> &response_images)
{
    double bin_width = M_PI / NUM_ORIENT_BINS;

    for (int bin_index = 0;bin_index < NUM_ORIENT_BINS;++bin_index)
    {
        double bin_min = (bin_index - 0.5) * bin_width;
        double bin_center = bin_index * bin_width;
        double bin_max = (bin_index + 0.5) * bin_width;

        for (int j = 0;j < IMAGE_SIZE;++j)
            for (int i = 0;i < IMAGE_SIZE;++i)
            {
                double cur_orientation = orientation(j, i);
                // fit into this bin
                if (cur_orientation > bin_min + my_eps &&
                    cur_orientation < bin_max - my_eps)
                {
                    double dis_from_center = std::abs(bin_center - cur_orientation) / bin_width;
                    response_images[bin_index](j, i) += (1.0 - dis_from_center) * l2_len(j, i);
                    if (cur_orientation < bin_center)
                    {
                        response_images[get_mod(bin_index - 1, NUM_ORIENT_BINS)](j, i) += dis_from_center * l2_len(j, i);
                    }
                }    
            }
    }
}

image_type apply_tent(image_type response_image, image_type &image_hist)
{
    complex_image_type complex_image, complex_result;
    fftwf_execute_dft_r2c(plan, (float *)(&response_image(0, 0)), reinterpret_cast<float(*)[2]>(&complex_image(0, 0)));
    // FFT for faster convolution
    complex_result = pointwise_multiply(complex_image, complex_tent);
    fftwf_execute_dft_c2r(inv_plan, reinterpret_cast<float(*)[2]>(&complex_image(0, 0)), &image_hist(0, 0));
    return image_hist;
}

// get gradient of the image with a Gaussian filter
vector<image_type> get_gaussian_gradient(image_type image)
{
    // Sobel operator to approximate the Gaussian filter
    dlib::matrix<float, 3, 3> sobel_x, sobel_y;
    image_type temp_image;
    sobel_x = -1.0, 0.0, 1.0,
              -2.0, 0.0, 2.0,
              -1.0, 0.0, 1.0;
    sobel_y = -1.0, -2.0, -1.0,
               0.0, 0.0, 0.0,
               1.0, 2.0, 1.0;
    image_type x_gradient = conv_same(image, sobel_x);
    image_type y_gradient = conv_same(image, sobel_y);
    vector<image_type> response_images, image_hist;
    response_images.assign(NUM_ORIENT_BINS, zeros_matrix(temp_image));
    image_hist.assign(NUM_ORIENT_BINS, zeros_matrix(temp_image));
    
    // calculate L2 length and orientation for all pixels
    image_type l2_len, orientation;
    for (int i = 0;i < IMAGE_SIZE;++i)
        for (int j = 0;j < IMAGE_SIZE;++j)
        {
            l2_len(i, j) = sqrt(x_gradient(i, j) * x_gradient(i, j) +
                                y_gradient(i, j) * y_gradient(i, j));
            orientation(i, j) = atan2(y_gradient(i, j), x_gradient(i, j));
            while (orientation(i, j) < 0 - my_eps)
                orientation(i, j) += M_PI;
            while (orientation(i, j) > M_PI + my_eps)
                orientation(i, j) -= M_PI;
        }
    get_response_images(l2_len, orientation, response_images);
    // apply tent to response images
    for (int i = 0;i < NUM_ORIENT_BINS;++i)
    {
        apply_tent(response_images[i], image_hist[i]); 
    }
    return image_hist;
}

// extract feature descriptors
vector<local_desc_type> extract_desc(vector<image_type> image_hist)
{
    // 28 * 28 sample points
    int sample_points_count = 28, sample_dis = IMAGE_SIZE / sample_points_count;
    vector<local_desc_type> feature_descs;

    for (int i = 0;i < NUM_ORIENT_BINS;++i)
        image_hist[i] += my_eps;
    for (int desc_center_x = sample_dis / 2; desc_center_x < IMAGE_SIZE;desc_center_x += sample_dis)
        for (int desc_center_y = sample_dis / 2; desc_center_y < IMAGE_SIZE;desc_center_y += sample_dis)
        {
            local_desc_type local_desc; 
            for (int o = 0;o < NUM_ORIENT_BINS;++o)
                for (int sy = 0;sy < NUM_SPATIAL_BINS;++sy)
                    for (int sx = 0;sx < NUM_SPATIAL_BINS;++sx)
                    {
                        int bin_center_x = spatial_bin_size / 2 + sx * spatial_bin_size - NUM_SPATIAL_BINS / 2 * spatial_bin_size;
                        int bin_center_y = spatial_bin_size / 2 + sy * spatial_bin_size - NUM_SPATIAL_BINS / 2 * spatial_bin_size;
                        // plus the offset
                        int real_x = desc_center_x + bin_center_x;
                        int real_y = desc_center_y + bin_center_y;
                        // index in the local feature vector
                        int local_index = o * NUM_SPATIAL_BINS * NUM_SPATIAL_BINS + sy * NUM_SPATIAL_BINS + sx;
                        // still in the image
                        if (real_x >= 0 && real_x < IMAGE_SIZE && real_y >= 0 && real_y < IMAGE_SIZE)
                        {
                            local_desc(local_index) = image_hist[o](real_x, real_y);
                        }
                    }
            feature_descs.push_back(normalize(local_desc));
        }
    return feature_descs;
}

// quantize against the visual vocab
void soft_quantize_feature(local_desc_type local_desc, vector<local_desc_type> vocab, local_desc_type &q)
{
    double sigma = 0.1;
    int n = vocab.size();
    for (int i = 0;i < n;++i)
    {
        local_desc_type dis = local_desc - vocab[i];
        q(i) = exp(-dot(dis, dis)) / (2 * sigma * sigma);
        // normalize q
        float sum = dlib::sum(q);
        if (sum > my_eps)
            q(i) /= sum;
    }
}

void build_sketch_hist(vector<local_desc_type> feature_descs, vector<local_desc_type> vocab, local_desc_type &sketch_hist)
{
    int n_desc = feature_descs.size();
    for (int i = 0;i < n_desc;++i)
    {
        local_desc_type q;
        soft_quantize_feature(feature_descs[i], vocab, q);
        sketch_hist += q;
    }
    sketch_hist /= n_desc;
}

vector<local_desc_type> extract_features_pipeline(image_type image)
{
    vector<image_type> image_hist = get_gaussian_gradient(image);
    vector<local_desc_type> feature_descs = extract_desc(image_hist);
    return feature_descs;
}
