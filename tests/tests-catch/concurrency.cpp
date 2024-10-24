#include <catch2/catch_test_macros.hpp>
#include <list>
#include <set>
#include <mutex>
#include <array>
#include <condition_variable>
#include <iostream>
#include <thread>

#include <dory/concurrency/log.h>
#include <dory/concurrency/messaging.h>
#include <dory/concurrency/worker.h>
#include <dory/concurrency/queue.h>
#include "../../components/testing/include/dory/testing/quickSort.h"
#include "../../components/testing/include/dory/testing/dataGenerators.h"
#include "../../components/testing/include/dory/testing/stack_lockfree.h"

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

struct QuitMessage
{
};

struct TestMessage
{
    int id;
};

TEST_CASE( "Single Element Queue", "[concurrency]" )
{
    dory::concurrency::messaging::SingleElementQueue<int> queue;

    REQUIRE(queue.empty());

    queue.emplace(1);

    REQUIRE(!queue.empty());

    auto value = queue.front();

    REQUIRE(value == 1);

    queue.pop();

    REQUIRE(queue.empty());

}

TEST_CASE( "Messaging main flow", "[concurrency]" )
{
    dory::concurrency::messaging::MultiMessageRecieverHub<TestMessage, QuitMessage> messageHub;

    auto sender = messageHub.getSender();

    sender.send(TestMessage{ 1 });
    sender.send(TestMessage{ 2 });

    bool firstMessageRecieved = false;
    bool secondMessageRecieved = false;
    bool thirdMessageRecieved = false;
    bool quitMessageRecieved = false;

    std::thread workerThread([&]()
    {
        messageHub.subscribe<TestMessage>([&](auto&& message)
        {
            if(message.id == 1)
            {
                firstMessageRecieved = true;
            }
            else if(message.id == 2)
            {
                secondMessageRecieved = true;
            }
            else if(message.id == 3)
            {
                thirdMessageRecieved = true;
            }

            return true;
        });

        messageHub.subscribe<QuitMessage>([&](auto&& message)
        {
            quitMessageRecieved = true;

            return false;
        });

        while(messageHub.wait())
        {}
    });

    sender.send(TestMessage{ 3 });
    sender.send(QuitMessage{});
    workerThread.join();

    REQUIRE(firstMessageRecieved);
    REQUIRE(secondMessageRecieved);
    REQUIRE(thirdMessageRecieved);
    REQUIRE(quitMessageRecieved);
}

TEST_CASE( "BoundedQueue invariants", "[concurrency]" )
{
    auto queue = dory::concurrency::BoundedQueue<int, 3>();

    REQUIRE(queue.push(1));
    REQUIRE(queue.push(2));
    REQUIRE(queue.push(3));
    REQUIRE(!queue.push(4));

    auto value = queue.pop();
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 1);

    REQUIRE(queue.push(4));

    value = queue.pop();
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 2);

    value = queue.pop();
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 3);

    value = queue.pop();
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 4);

    REQUIRE(queue.push(5));

    value = queue.pop();
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 5);
}

TEST_CASE( "BoundedQueue isEmpty", "[concurrency]" )
{
    auto queue = dory::concurrency::BoundedQueue<int, 4>();

    REQUIRE(queue.isEmpty());
}

TEST_CASE( "BoundedQueue isFull", "[concurrency]" )
{
    auto queue = dory::concurrency::BoundedQueue<int, 4>();

    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);

    REQUIRE(queue.isFull());
}

TEST_CASE( "BoundedQueue concurrent usage", "[concurrency]" )
{
    auto queue = dory::concurrency::BoundedQueue<int, 4>();

    std::thread pushThread1([&]()
    {
        REQUIRE(queue.push(1));
    });

    std::thread pushThread2([&]()
    {
        REQUIRE(queue.push(2));
    });

    std::thread popThread1([&]()
    {
        std::set<int> values;

        while(true)
        {
            auto head = queue.waitAndPop();
            if(head.has_value())
            {
                auto result = values.insert(head.value());
                REQUIRE(result.second);
                if(values.size() == 2)
                {
                    break;
                }
            }
        }

        REQUIRE(values.size() == 2);
        for(auto value : values)
        {
            auto condition = value == 1 || value == 2;
            REQUIRE(condition);
        }
    });

    pushThread1.join();
    pushThread2.join();
    popThread1.join();
}

TEST_CASE( "BoundedQueue concurrent usage with many threads", "[concurrency]" )
{
    auto queue = dory::concurrency::BoundedQueue<int, 4>();
    std::atomic<bool> startFlag;
    std::condition_variable startCondition;
    std::mutex startMutex;

    std::thread pushThread1([&]()
    {
        std::unique_lock<std::mutex> lock(startMutex);
        startCondition.wait(lock, [&](){return startFlag.load();});

        REQUIRE(queue.waitAndPush(1));
        REQUIRE(queue.waitAndPush(2));
        REQUIRE(queue.waitAndPush(3));
    });

    std::thread pushThread2([&]()
    {
        std::unique_lock<std::mutex> lock(startMutex);
        startCondition.wait(lock, [&](){return startFlag.load();});

        REQUIRE(queue.waitAndPush(4));
        REQUIRE(queue.waitAndPush(5));
        REQUIRE(queue.waitAndPush(6));
    });

    std::array<bool, 6> values {false, false, false, false, false, false};
    std::atomic<std::size_t> valuesCount = 0;

    std::thread popThread1([&]()
    {
        while(valuesCount < 6)
        {
            auto head = queue.pop();
            if(head.has_value())
            {
                auto value = head.value();
                values[value - 1] = true;
                valuesCount++;
            }
        }
    });

    std::thread popThread2([&]()
    {
       while(valuesCount < 6)
       {
           auto head = queue.pop();
           if(head.has_value())
           {
               auto value = head.value();
               values[value - 1] = true;
               valuesCount++;
           }
       }
    });

    startFlag = true;
    startCondition.notify_all();

    pushThread1.join();
    pushThread2.join();
    popThread1.join();
    popThread2.join();

    for(auto value : values)
    {
        REQUIRE(value);
    }

    REQUIRE(queue.isEmpty());
}

TEST_CASE( "Lockfree stack - book", "[concurrency]" )
{
    auto stack = dory::testing::lock_free_stack<int>();
    stack.push(1);
    auto value = stack.pop();

    REQUIRE(value != nullptr);
    REQUIRE(*value == 1);
}