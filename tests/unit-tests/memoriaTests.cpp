#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/fmt/fmt.h>
#include <dory/memory/allocation.h>
#include <backward.hpp>

#if DORY_PLATFORM_LINUX
#include <sys/mman.h>
#include <emmintrin.h>

TEST(MemoriaTests, allocate)
{
    size_t page_size = sysconf(_SC_PAGESIZE);  // Get system page size
    void* ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap failed");
    }

    // Use memory here
    ((char*)ptr)[0] = 'A';

    // Free memory
    munmap(ptr, page_size);
}

TEST(MemoriaTests, pageTableHierarchy)
{
    constexpr std::size_t addressSpacePower = 64;
    constexpr std::size_t wordSizePower = 4;
    constexpr std::size_t pageSizePower = 12;
    constexpr std::size_t pageLevelsCount = 4;

    std::size_t entriesCount = addressSpacePower - pageSizePower;

    for(std::size_t i = 0; i < pageLevelsCount; ++i)
    {
        std::cout << fmt::format("Level {} PT: size {} MiB, {} PTEs", i + 1, std::pow(2, entriesCount + wordSizePower - 20), std::pow(2, entriesCount)) << std::endl;
        entriesCount = entriesCount - pageSizePower;
    }
}

void setbytes(char* p, int c)
{
    __m128i i = _mm_set_epi8(c, c, c, c,
                            c, c, c, c,
                            c, c, c, c,
                            c, c, c, c);
    _mm_stream_si128((__m128i*)&p[0], i);
    _mm_stream_si128((__m128i*)&p[16], i);
    _mm_stream_si128((__m128i*)&p[32], i);
    _mm_stream_si128((__m128i*)&p[48], i);
}

TEST(MemoriaTests, fillMemory)
{
    size_t page_size = sysconf(_SC_PAGESIZE);  // Get system page size
    void* ptr = std::aligned_alloc(alignof(__m128i), page_size);


    setbytes((char*)ptr, 0);

    const char* cptr = (char*)ptr;

    for(std::size_t i = 0; i < 64; ++i)
    {
        std::cout << (int)cptr[i] << " ";
    }

    std::cout << std::endl;
}

struct alignas(64) TestType
{
    int id{};
    int count {};
    std::size_t fill[7];
};

TestType* makeTestType() {
    return new TestType{};
}

TEST(MemoriaTests, typeSizes)
{
    std::cout << fmt::format("int size: {}, alignment: {}", sizeof(int), alignof(int)) << std::endl;
    std::cout << fmt::format("std::size_t size: {}, alignment: {}", sizeof(std::size_t), alignof(std::size_t)) << std::endl;
    std::cout << fmt::format("double size: {}, alignment: {}", sizeof(double), alignof(double)) << std::endl;
    std::cout << fmt::format("long double size: {}, alignment: {}", sizeof(long double), alignof(long double)) << std::endl;
    std::cout << fmt::format("TestType size: {}, alignment: {}", sizeof(TestType), alignof(TestType)) << std::endl;

    void* ptr = std::aligned_alloc(alignof(TestType), sizeof(TestType));
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(TestType), 0);
    std::free(ptr);

    void* ptr2 = std::malloc(sizeof(TestType));
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr2) % alignof(TestType), 0);
    std::free(ptr2);
}

void printStack()
{
    namespace bw = backward;

    bw::StackTrace st;
    st.load_here(64);  // capture up to 64 frames

    bw::TraceResolver resolver;
    resolver.load_stacktrace(st);

    for (int i = static_cast<int>(st.size()) - 1; i >= 0; --i)
    {
        const bw::ResolvedTrace trace = resolver.resolve(st[i]);
        std::cout << "#" << i << " ";

        if (!trace.source.filename.empty())
        {
            std::cout << trace.source.filename << ":"
                      << trace.source.line << " - ";
        }

        std::cout << trace.object_function << std::endl;
    }
}

TEST(MemoriaTests, alignAddress)
{
    dory::assert::Assert::assertFailureHandler = [](const char * msg) {
        std::cerr << msg << std::endl;
    };

    dory::assert::InHouseAssert::assertFailureHandler = [](const char * msg) {
        std::cerr << msg << std::endl;
        doryDebugBreak();
    };

    dory::assert::DebugAssert::assertFailureHandler = [](const char * msg) {
        std::cerr << msg << std::endl;
        printStack();
        doryDebugBreak();
        std::exit(EXIT_FAILURE);
    };

    dory::memory::alignAddress(12, 8);
}

#endif