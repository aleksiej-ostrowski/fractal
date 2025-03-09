/* ————————————————————————————————————————————————————————————*
 * Computational experiment with pivot element                 *
 * version 0.01                                                *
 * (c) Aleksei Ostrovskii, aleksiej.ostrowski@gmail.com, 2025  *
 * ————————————————————————————————————————————————————————————*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define MAX_VAL 1000000
#define CACHE_FLUSH_SIZE (32 * 1024 * 1024) // 32 MB

#define MAGIC_CONST 0.749

ssize_t max(ssize_t a, ssize_t b) {
    return a > b ? a : b;
}

void count_inversions(int *arr, ssize_t array_size, ssize_t *pl_count, ssize_t *mn_count, ssize_t *total_pairs) {

   *pl_count    = 0;
   *mn_count    = 0;
   *total_pairs = 0;

   for (ssize_t ind = 0; ind < array_size - 1; ind++)
        for (ssize_t iter = ind + 1; iter < array_size; iter++) {
            (*total_pairs)++;
            if (arr[ind] > arr[iter])
                (*pl_count)++;
            else
            if (arr[ind] < arr[iter])
                (*mn_count)++;
        }
}

double sort_degree(int *arr, ssize_t start, ssize_t end) {

    ssize_t array_size = end - start + 1;

    if (array_size < 2) return 1.;

    int *slice = (int*)malloc(array_size * sizeof(int));

    if (!slice) {
        perror("Memory allocation failed");
        exit(1);
    }

    for (ssize_t iter = 0; iter < array_size; iter++)
        slice[iter] = arr[start + iter];

    ssize_t pl_count, mn_count, total_pairs;

    count_inversions(slice, array_size, &pl_count,
                                        &mn_count,
                                        &total_pairs);

    free(slice);

    if (!total_pairs) return 0.;

    ssize_t max_inversions = max(total_pairs - pl_count,
                                 total_pairs - mn_count);

    return (double) max_inversions / total_pairs;
}


double uniform_random() {
    return (double)rand() / (RAND_MAX + 1.);
}

int generate_uniform_random(int min, int max) {
    return min + (int)(uniform_random() * (max - min + 1));
}

int generate_benford_random(int min, int max) {
    if (min <= 0) min = 1;
    double log_min = log(min);
    double log_max = log(max);
    double res;
    do {
        double u = uniform_random();
        res = exp(u * (log_max - log_min) + log_min);
    } while (res < min || res > max);
    return (int)res;
}

int generate_exponential_random(int min, int max) {
    double lambda = log(1e6) / max;
    double res;
    do {
        double u = uniform_random();
        res = -log(1. - u) / lambda;
    } while (res < min || res > max);
    return (int)res;
}

int generate_cauchy_random(int min, int max) {
    double x0    = max / 2.;
    double gamma = max / 6.;
    double res;
    do {
        double u = uniform_random();
        res = x0 + gamma * tan(M_PI * (u - 0.5));
    } while (res < min || res > max);
    return (int)res;
}

void flush_cache() {
    uint8_t *buffer = (uint8_t *)malloc(CACHE_FLUSH_SIZE);

    if (buffer == NULL) {
        perror("Failed to allocate buffer");
        return;
    }

    for (volatile ssize_t iter = 0; iter < CACHE_FLUSH_SIZE; iter++)
        buffer[iter] = iter & 0xFF;

    free(buffer);
}

uint8_t is_sorted(int *arr, ssize_t n) {

    if (n <= 1) return 1;

    uint8_t ascending  = 1;
    uint8_t descending = 1;

    for (ssize_t iter = 0; iter < n - 1; iter++) {
        if (arr[iter] > arr[iter + 1])
            ascending = 0;
        if (arr[iter] < arr[iter + 1])
            descending = 0;
        if (!ascending && !descending) break;
    }

    return ascending || descending;
}


void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int ascending(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int descending(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

int *generate_array(ssize_t n, uint8_t dist, char *order, uint8_t shuffle, uint8_t twins) {
    int *arr = (int*)malloc(n * sizeof(int));

    int (*random_func)(int, int) = NULL;

    switch (dist) {
        case 1: random_func = generate_uniform_random; break;
        case 2: random_func = generate_exponential_random; break;
        case 3: random_func = generate_benford_random; break;
        case 4: random_func = generate_cauchy_random; break;
    }

    for (ssize_t iter = 0; iter < n; iter++)
        arr[iter] = random_func(0, MAX_VAL);

    if (strcmp(order, "sorted") == 0)
        qsort(arr, n, sizeof(int), ascending);
    else
    if (strcmp(order, "reversed") == 0)
        qsort(arr, n, sizeof(int), descending);

    if (shuffle) {
        ssize_t num_swaps = (n * shuffle) / 100;
        for (ssize_t iter = 0; iter < num_swaps; iter++) {
            ssize_t idx1 = generate_uniform_random(0, n - 1);
            ssize_t idx2 = generate_uniform_random(0, n - 1);
            swap(arr+idx1, arr+idx2);
        }
    }

    if (twins) {
        ssize_t num_swaps = (n * twins) / 100;
        for (ssize_t iter = 0; iter < num_swaps; iter++) {
            ssize_t idx1 = generate_uniform_random(0, n - 1);
            ssize_t idx2 = generate_uniform_random(0, n - 1);
            arr[idx1] = arr[idx2];
        }
    }

    return arr;
}

void my_qsort(int *arr, ssize_t start, ssize_t end, int p_method, double *pivot_calc_time, ssize_t *rec, ssize_t *chg);


int median(int *arr, ssize_t start, ssize_t end) {
    ssize_t length = end - start + 1;
    int *slice = (int*)malloc(length * sizeof(int));

    if (!slice) {
        perror("Memory allocation failed");
        exit(1);
    }

    for (ssize_t iter = 0; iter < length; iter++)
        slice[iter] = arr[start + iter];

    qsort(slice, length, sizeof(int), ascending);

    ssize_t na_dwa = length >> 1;
    int median_value = (length % 2 != 0) ? slice[na_dwa]
                                         : (slice[na_dwa - 1] +
                                            slice[na_dwa]) / 2;

    free(slice);
    return median_value;
}

void my_qsort(int *arr, ssize_t start, ssize_t end, int p_method, double *pivot_calc_time, ssize_t *rec, ssize_t *chg) {
    if (start >= end) return;

    (*rec)++;

    clock_t start_time = clock();

    int pivot = arr[start];

    if (p_method == 1) {
        pivot = arr[generate_uniform_random(start, end)];
    } else
    if (p_method == 2) {
        pivot = median(arr, start, end);
    } else
    if (p_method == 3) {
        double cr = sort_degree(arr, start, end);
        if (cr > MAGIC_CONST)
            pivot = median(arr, start, end);
        else
            pivot = arr[generate_uniform_random(start, end)];
    }

    *pivot_calc_time += (double)(clock() - start_time) / CLOCKS_PER_SEC;

    ssize_t low = start, high = end;

    while (low <= high) {
        while (arr[low]  < pivot) low++;
        while (arr[high] > pivot) high--;
        if (low <= high) {
            swap(arr + low, arr + high);
            low++;
            high--;
            (*chg)++;
        }
    }

    if (start < high) my_qsort(arr, start, high, p_method, pivot_calc_time, rec, chg);
    if (low < end)    my_qsort(arr, low,   end,  p_method, pivot_calc_time, rec, chg);

}


char* red_or_green(uint8_t a) {
    if (a)
        return "\033[0;32mДА\033[0m";
    else
        return "\033[0;31mНЕТ\033[0m";
}

int main(int argc, char* argv[]) {

    if (argc != 8) {
        printf("Запуск: %s <N> <RUNS> "
                        "<uniform|exp|benford|cauchy> "
                        "<regular|sorted|reversed> "
                        "<0-100> <0-100> <0|1|2|3>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    printf("N = %d \n", N);

    int RUNS = atoi(argv[2]);
    printf("RUNS = %d \n", RUNS);

    char* dist = argv[3];
    printf("dist = %s \n", dist);

    char* order = argv[4];
    printf("order = %s \n", order);

    if (strcmp(order, "sorted")    &&
        strcmp(order, "reversed")  &&
        strcmp(order, "regular") ) {
        fprintf(stderr, "Invalid order: %s\n", order);
        return 2;
    }

    int p_shuffle = atoi(argv[5]);

    if (p_shuffle < 0 || p_shuffle > 100) {
        fprintf(stderr, "Invalid shuffle value\n");
        return 3;
    }

    printf("shuffle = %d\n", p_shuffle);

    int p_twins = atoi(argv[6]);

    if (p_twins < 0 || p_twins > 100) {
        fprintf(stderr, "Invalid twins value\n");
        return 4;
    }

    printf("twins = %d\n", p_twins);

    int p_method = atoi(argv[7]);
    printf("p_method = %d\n", p_method);

    if (p_method < 0 || p_method > 3) {
        fprintf(stderr, "Invalid pivot method\n");
        return 5;
    }

    uint8_t dist_int = 0;
    if      (!strcmp(dist, "uniform")) dist_int = 1;
    else if (!strcmp(dist, "exp")    ) dist_int = 2;
    else if (!strcmp(dist, "benford")) dist_int = 3;
    else if (!strcmp(dist, "cauchy"))  dist_int = 4;

    if (dist_int < 1 || dist_int > 4) {
        fprintf(stderr, "Invalid distribution: %s\n", dist);
        return 6;
    }

    double sort_degree_global = 0.;
    double time_res_global    = 0.;
    ssize_t res_rec_global    = 0;
    ssize_t res_chg_global    = 0;

    for (int run = 0; run < RUNS; run++) {

        srand(200 + run);

        int *arr = generate_array(N, dist_int, order, p_shuffle, p_twins);
        sort_degree_global += sort_degree(arr, 0, N - 1);

        // printf("Исходный массив отсортирован: %s\n", red_or_green(is_sorted(arr, N)));

        flush_cache();

        double pivot_calc_time = 0.;

        ssize_t rec = 0;
        ssize_t chg = 0;

        clock_t start_time = clock();

        my_qsort(arr, 0, N - 1, p_method, &pivot_calc_time, &rec, &chg);

        double res_time = (double)(clock() - start_time) / CLOCKS_PER_SEC - pivot_calc_time;

        uint8_t sorted = is_sorted(arr, N);

        if (!sorted)
            printf("Результ отсортирован: %s\n", red_or_green(!sorted));

        time_res_global += res_time;
        res_rec_global  += rec;
        res_chg_global  += chg;

        free(arr);
    }

    printf("Среднее время                  : %.5f сек.\n", time_res_global / RUNS);
    printf("Среднее число рекурсий         : %ld\n",       res_rec_global  / RUNS);
    printf("Среднее число обменов          : %ld\n",       res_chg_global  / RUNS);
    printf("Средняя \"отсортированность\"    : %.5f\n",    sort_degree_global  / RUNS);


    return 0;
}

