#include <vector>
template<class T>
double get_max(T matrix)
{
    double max_element = 0.0;
    for (int i = 0;i < matrix.NR;++i)
        for (int j = 0;j < matrix.NC;++j)
            if (matrix(i, j) > max_element)
                max_element = matrix(i, j);
    return max_element;
}

template<class T>
void print_matrix(T matrix)
{
    for (int i = 0;i < matrix.NR;++i)
    {
        for (int j = 0;j < matrix.NC;++j)
            if (matrix(i, j) < 100)
                printf("%.6lf ", matrix(i, j));
        printf("\n");
    }
}
template <class T>
void print_vec(std::vector<T> matrix_list)
{
    int n = matrix_list.size();
    for (int i = 0;i < n;++i)
    {
        printf("vector index %d:\n", i);
        print_matrix(matrix_list[i]);
    }
}
