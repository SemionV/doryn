export module dory.core.typeTraits;

export namespace dory::core::typeTraits
{
    class NonCopyable
    {
    public:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

        NonCopyable() = default;
    };
}