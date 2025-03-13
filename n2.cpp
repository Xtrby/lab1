#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <random>

int shared_balance_no_sync = 0;
std::atomic<int> shared_balance_atomic(0);
int shared_balance_mutex = 0;
std::mutex balance_mutex_lock;

void perform_transaction_no_sync(int client_id) {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(-100, 100);

    for (int i = 0; i < 1000; ++i) {
        int transaction_amount = distribution(generator);
        shared_balance_no_sync += transaction_amount;
    }
}

void perform_transaction_atomic(int client_id) {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(-100, 100);

    for (int i = 0; i < 1000; ++i) {
        int transaction_amount = distribution(generator);
        shared_balance_atomic += transaction_amount;
    }
}

void perform_transaction_mutex(int client_id) {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(-100, 100);

    for (int i = 0; i < 1000; ++i) {
        int transaction_amount = distribution(generator);
        std::lock_guard<std::mutex> lock(balance_mutex_lock);
        shared_balance_mutex += transaction_amount;
    }
}

void execute_test(int thread_count, const std::string& test_label, void (*transaction_func)(int)) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> client_threads;
    for (int i = 0; i < thread_count; ++i) {
        client_threads.emplace_back(transaction_func, i);
    }

    for (auto& t : client_threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << "test " << test_label << " (streams: " << thread_count << ")\n";
    std::cout << "  time: " << duration.count() << " sec\n";
    if (test_label == "w/o sync") {
        std::cout << "  total: " << shared_balance_no_sync << "\n";
    } else if (test_label == "using std::atomic") {
        std::cout << "  total: " << shared_balance_atomic << "\n";
    } else if (test_label == "using std::mutex") {
        std::cout << "  total: " << shared_balance_mutex << "\n";
    }
    std::cout << "\n";
}

int main() {
    std::vector<int> thread_counts = {2, 4, 8};

    for (int thread_count : thread_counts) {
        shared_balance_no_sync = 0;
        shared_balance_atomic = 0;
        shared_balance_mutex = 0;
        execute_test(thread_count, "w/o sync", perform_transaction_no_sync);
        execute_test(thread_count, "using std::atomic", perform_transaction_atomic);
        execute_test(thread_count, "using std::mutex", perform_transaction_mutex);
    }

    return 0;
}