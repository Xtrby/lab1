#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <chrono>

const int N = 1000;
std::vector<std::vector<int>> A(N, std::vector<int>(N, 1));
std::vector<std::vector<int>> B(N, std::vector<int>(N, 1));
std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

void multiply_row(int row_index) {
    for (int col_index = 0; col_index < N; ++col_index) {
        for (int k = 0; k < N; ++k) {
            C[row_index][col_index] += A[row_index][k] * B[k][col_index];
        }
    }
}

void parallel_multiply(int thread_count) {
    boost::thread_group thread_pool;
    for (int i = 0; i < N; i += N / thread_count) {
        for (int row_index = i; row_index < i + N / thread_count; ++row_index) {
            thread_pool.create_thread(boost::bind(multiply_row, row_index));
        }
    }
    thread_pool.join_all();
}

void single_thread_multiply() {
    for (int row_index = 0; row_index < N; ++row_index) {
        for (int col_index = 0; col_index < N; ++col_index) {
            for (int k = 0; k < N; ++k) {
                C[row_index][col_index] += A[row_index][k] * B[k][col_index];
            }
        }
    }
}

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    single_thread_multiply();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> single_thread_duration = end_time - start_time;

    std::vector<int> thread_counts = {2, 4, 8};
    std::vector<double> parallel_durations;

    for (int thread_count : thread_counts) {
        C = std::vector<std::vector<int>>(N, std::vector<int>(N, 0));

        auto start_time = std::chrono::high_resolution_clock::now();
        parallel_multiply(thread_count);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> parallel_duration = end_time - start_time;
        parallel_durations.push_back(parallel_duration.count());
    }

    std::cout << "N = " << N << ":\n";
    std::cout << "single-threaded exec: " << single_thread_duration.count() << " sec\n";
    for (size_t i = 0; i < thread_counts.size(); ++i) {
        std::cout << "multithreaded exec (threads: " << thread_counts[i] << "): " << parallel_durations[i] << " sec\n";
    }

    return 0;
}