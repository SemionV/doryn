#pragma once

namespace dory::domain
{
    template<typename TId>
    class IIdFactory
    {
        public:
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
                return entity::nullId;;
            }
    };
}