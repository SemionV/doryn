#include "dependencies.h"
#include "base/concurrency/threadPool.h"
#include "base/concurrency/log.h"
#include "base/testing/dataGenerators.h"
#include "base/testing/quickSort.h"
#include "base/concurrency/messaging.h"

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

TEST_CASE( "sequential quick sort", "[.][concurrency]" )
{
    auto data = std::list<int>{5, 7, 3, 4, 1, 9, 2, 8, 10, 6};

    print(data);

    auto beginTime = std::chrono::high_resolution_clock::now();

    auto dataSorted = dory::testing::quickSort(data);

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

TEST_CASE( "worker test", "[.][concurrency]" )
{
    using Log = dory::concurrency::logging::Log<decltype(std::cout)>;
    auto log = Log(std::cout);

#ifdef NDEBUG
    using WorkerProfilePolicies = dory::concurrency::WorkerDefaultProfilePolicy;
#else
    using WorkerProfilePolicies = dory::concurrency::WorkerLogProfilePolicy;
#endif

    constexpr static const std::size_t elementsCount = 100000;
    auto worker = dory::concurrency::Worker<Log, WorkerProfilePolicies, void, std::array<int, elementsCount>&>(log, 2);

    auto data = dory::testing::getArray<int, elementsCount>();
    auto data2 = dory::testing::getArray<int, elementsCount>();
    auto data3 = dory::testing::getArray<int, elementsCount>();

    auto futureResult = worker.addTask(sort, *data2);
    auto futureResult2 = worker.addTask(sort, *data3);

    sort(*data);
    WorkerProfilePolicies::print(log, "get future start");
    futureResult.get();
    WorkerProfilePolicies::print(log, "get future end");
    WorkerProfilePolicies::print(log, "get future 2 start");
    futureResult2.get();
    WorkerProfilePolicies::print(log, "get future 2 end");
}

struct TestMessage
{
    int id;
};

TEST_CASE( "MessageQueue: basic flow", "[concurrency]" )
{
    dory::concurrency::messaging::MessageQueue<TestMessage> messageQueue;

    int idResult = 0;

    auto future = std::async(std::launch::async, [&idResult, &messageQueue]()
    {
        auto message = messageQueue.waitForMessage();
        idResult = message.id;
    });

    messageQueue.pushMessage(TestMessage{ 1 });

    future.wait_for(std::chrono::milliseconds(10));

    REQUIRE(idResult == 1);
}