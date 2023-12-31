#include "dependencies.h"
#include "base/concurrency/threadPool.h"

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

template<typename T>
auto getDataForStdSort()
{
    return std::array<int, 10>{5, 7, 3, 4, 1, 9, 2, 8, 10, 6};
}

TEST_CASE( "std::ranges sort", "[concurrency]" )
{
    auto data = getDataForStdSort<int>();

    print(data);

    std::ranges::sort(data, std::ranges::less());

    print(data);
}

TEST_CASE( "worker", "[concurrency]" )
{
    auto worker = dory::concurrency::Worker<2, int>();
}