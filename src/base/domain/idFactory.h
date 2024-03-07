#pragma once

namespace dory::domain
{
    template<typename TId>
    class IIdFactory
    {
        public:
            virtual ~IIdFactory() = default;

            virtual TId generate() = 0;
            virtual TId getNullId() = 0;
    };

    template<typename TId>
    class NumberIdFactory: public IIdFactory<TId>
    {
        private:
            TId counter;

        public:
            TId generate() override
            {
                return ++counter;
            }

            TId getNullId() override
            {
                return dory::entity::nullId;;
            }
    };

    template<typename TId>
    class IdFactory
    {
    private:
        TId counter;

    public:
        static const constexpr TId nullId{};

    public:
        TId generate()
        {
            return ++counter;
        }
    };
}