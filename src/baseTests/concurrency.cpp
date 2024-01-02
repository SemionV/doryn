#include "dependencies.h"
#include "base/concurrency/threadPool.h"
#include "base/testing/dataGenerators.h"
#include "base/testing/quickSort.h"

TEST_CASE( "Get number of CPU cores", "[.][concurrency]" )
{
    std::cout << "CPU Cores: " << std::thread::hardware_concurrency() << std::endl;
}

template<typename TCollection>
void print(const TCollection& collection)
{
    std::cout << "[";
    bool isFirstItem = true;
    for(const auto& item : collection)
    {
        if(!isFirstItem)
        {
            std::cout << ", ";
        }
        else
        {
            isFirstItem = false;
        }
        std::cout << item;
    }
    std::cout << "]" << std::endl;
}

TEST_CASE( "std::partition", "[.][concurrency]" )
{
    auto data = std::list<int>{5, 7, 3, 4, 1, 9, 2, 8, 10, 6};

    print(data);

    auto position = std::partition(data.begin(), data.end(), [](const auto& x)
    {
        return x < 5;
    });

    print(data);

    std::cout << "position: " << *position << std::endl;
}

template<typename T>
std::list<T> quickSort(std::list<T> collection)
{
    if(collection.empty())
    {
        return collection;
    }

    auto collectionSorted = std::list<T>{};
    collectionSorted.splice(collectionSorted.begin(), collection, collection.begin());

    const auto& pivotValue = *collectionSorted.begin();

    auto partitionRange = std::ranges::partition(collection, [&pivotValue](const T& item)
    {
        return item < pivotValue;
    });

    auto lowerHalfCollection = std::list<T>{};
    lowerHalfCollection.splice(lowerHalfCollection.end(), collection, collection.begin(), partitionRange.begin());

    auto lowerHalfCollectionSorted = quickSort(std::move(lowerHalfCollection));
    auto higherHalffCollectionSorted = quickSort(std::move(collection));

    collectionSorted.splice(collectionSorted.end(), higherHalffCollectionSorted);
    collectionSorted.splice(collectionSorted.begin(), lowerHalfCollectionSorted);

    return collectionSorted;
}

TEST_CASE( "sequential quick sort", "[.][concurrency]" )
{
    auto data = std::list<int>{5, 7, 3, 4, 1, 9, 2, 8, 10, 6};

    print(data);

    auto beginTime = std::chrono::high_resolution_clock::now();

    auto dataSorted = quickSort(data);

    auto endTime = std::chrono::high_resolution_clock::now();

    print(dataSorted);

    auto duration = std::chrono::duration<double>(endTime - beginTime);

    std::cout << "time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << " microseconds" << std::endl;
}

TEST_CASE( "std::ranges sort", "[.][concurrency]" )
{
    auto data = dory::testing::getArray<int, 10>();

    print(*data);

    std::ranges::sort(*data, std::ranges::less());

    print(*data);
}

void sort(std::array<int, 100000>& collection)
{
    std::cout << std::this_thread::get_id() << ": sort" << "\n";
    std::ranges::sort(collection, std::ranges::less());
}

/*
 * Proper number of threads running
 * Threads are invoking tasks
 * There can be more tasks than threads
 * Task queue is properly handled
 */

TEST_CASE( "worker test", "[concurrency]" )
{
    auto worker = dory::concurrency::Worker<void, std::array<int, 100000>&>(2);

    auto data = dory::testing::getArray<int, 100000>();
    auto data2 = dory::testing::getArray<int, 100000>();
    auto data3 = dory::testing::getArray<int, 100000>();

    auto futureResult = worker.addTask(sort, *data2);
    auto futureResult2 = worker.addTask(sort, *data3);

    sort(*data);
    std::cout << std::this_thread::get_id() << ": get future start" << "\n";
    futureResult.get();
    std::cout << std::this_thread::get_id() << ": get future end" << "\n";
    std::cout << std::this_thread::get_id() << ": get future 2 start" << "\n";
    futureResult2.get();
    std::cout << std::this_thread::get_id() << ": get future 2 end" << "\n";
}