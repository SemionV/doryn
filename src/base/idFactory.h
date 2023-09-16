#pragma once

namespace dory
{
    template<typename TId>
    class IIdFactory
    {
        public:
            virtual TId generate() = 0;
    };

    template<typename TId>
    class NumberIdFactory: public IIdFactory<TId>
    {
        private:
            TId counter;

        public:
            TId generate()
            {
                return ++counter;
            }
    };
}