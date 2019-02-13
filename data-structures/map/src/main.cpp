#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <iostream>
#include <vector>

#include "TreeMap.h"
#include "HashMap.h"

using Tree = aisdi::TreeMap<int, int>;
using Hash = aisdi::HashMap<int, int>;
#define clock std::chrono::system_clock
#define duration std::chrono::duration<int64_t, std::nano>
#define time_unit std::chrono::duration_cast<std::chrono::nanoseconds>

void prepare_tree(Tree &tree, size_t n)
{
    for (size_t i = 1; i <= n; i++)
    {
        tree[i] = i;
    }
}

void prepare_hash(Hash &hash, size_t n)
{
    for (size_t i = 1; i <= n; i++)
    {
        hash[i] = i;
    }
}

void insert_tree(Tree &tree, int n)
{
    tree[n + 1] = n + 1;
}
void insert_hash(Hash &hash, int n)
{
    hash[n + 1] = n + 1;
}

void find_tree(Tree &tree, int n)
{
    tree.find(n);
}

void find_hash(Hash &hash, int n)
{
    hash.find(n);
}

void remove_tree(Tree &tree, int n)
{
    tree.remove(n);
}

void remove_hash(Hash &hash, int n)
{
    hash.remove(n);
}

void value_tree(Tree &tree, int n)
{
    tree.valueOf(n);
}
void value_hash(Hash &hash, int n)
{
    hash.valueOf(n);
}

void perform_test(
    size_t repeat,
    void (*tree_operation)(Tree &, int),
    void (*hash_operation)(Hash &, int)
)
{
    std::vector<size_t> sizes = {10, 1000, 10000, 55000, 100000, 200000, 500000};

    for (auto n : sizes)
    {
        auto key = 1;
        size_t avg_tree = 0;
        size_t avg_hash = 0;
        for (size_t r = 0; r < repeat; r++)
        {
            aisdi::TreeMap<int, int> tree;
            prepare_tree(tree, n);

            auto tree_start = clock::now();
            tree_operation(tree, key);
            auto tree_end = clock::now();
            auto tree_time = time_unit(tree_end - tree_start);
            avg_tree += tree_time.count();
            aisdi::HashMap<int, int> hash;
            prepare_hash(hash, n);

            auto hash_start = clock::now();
            hash_operation(hash, key);
            auto hash_end = clock::now();
            auto hash_time = time_unit(hash_end - hash_start);
            avg_hash += hash_time.count();
        }
        std::cout << n << ',' << avg_tree / (double)repeat << ',' << avg_hash / (double)repeat << '\n';
    }
}

int main(int argc, char **argv)
{

    srand(time(NULL));
    std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 1;

    std::cout<<"\ninsert\n";
    perform_test(repeatCount, insert_tree, insert_hash);

    std::cout<<"\nfind\n";
    perform_test(repeatCount, find_tree, find_hash);

    std::cout<<"\nremove\n";
    perform_test(repeatCount, remove_tree, remove_hash);

    std::cout<<"\nvalueof\n";
    perform_test(repeatCount, value_tree, value_hash);
    return 0;
}
