#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

namespace managing::threads::examples {

template <typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        //std::cout << std::this_thread::get_id() << " calculating...\n";
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    const auto length = std::distance(first, last);

    if (!length)
        return init;

    const auto min_entries_per_thread = 25;
    const auto max_threads =
        (length + min_entries_per_thread - 1) / min_entries_per_thread;

    const auto hardware_threads = std::thread::hardware_concurrency();
    const auto num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2l, max_threads);

    std::cout << "hardware_threads=" << hardware_threads
        << " number_threads=" << num_threads << '\n';

    // number of entries for each thread to process
    const auto block_size = length / num_threads;

    std::vector<T> results(num_threads);
    // main thread is one thread, so you should spawn num_threads - 1
    std::vector<std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(accumulate_block<Iterator, T>(),
                                 block_start, block_end,
                                 std::ref(results[i]));
        block_start = block_end;
    }
    // main thread handle all the remaining entries
    accumulate_block<Iterator, T>()(block_start, last,
                                    results[num_threads - 1]);

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);
}

void parallelAccumulateDemo() {
    std::vector<int> vi;
    for (int i = 0; i < 1000; ++i) {
        vi.push_back(10);
    }
    int sum = parallel_accumulate(vi.begin(), vi.end(), 5);
    std::cout << "sum=" << sum << std::endl;
}

} // managing::threads::examples