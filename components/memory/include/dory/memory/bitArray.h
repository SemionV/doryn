#pragma once
#include <cstddef>
#include <climits>
#include <dory/macros/assert.h>

namespace dory::memory
{
    template<typename TWord>
    requires(std::is_integral_v<TWord>)
    class BitArray
    {
    private:
        const std::size_t _wordsCount;
        const std::size_t _bitsPerWord;
        const std::size_t _bitsCount;
        TWord* _words;

    public:
        static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

        explicit BitArray(const std::size_t wordsCount, TWord* words) noexcept:
        _wordsCount(wordsCount),
        _bitsPerWord(sizeof(TWord) * CHAR_BIT),
        _bitsCount(_bitsPerWord * wordsCount),
        _words(words)
        {}

        void set(const std::size_t index)
        {
            assert::debug(index < _bitsCount, "Invalid index");
            _words[index / _bitsPerWord] |= TWord(1) << index % _bitsPerWord;
        }

        void clear(const std::size_t index)
        {
            assert::debug(index < _bitsCount, "Invalid index");
            _words[index / _bitsPerWord] &= ~(TWord(1) << (index % _bitsPerWord));
        }

        [[nodiscard]] bool get(const std::size_t index) const
        {
            assert::debug(index < _bitsCount, "Invalid index");
            return (_words[index / _bitsPerWord] >> (index % _bitsPerWord)) & 1;
        }

        [[nodiscard]] std::size_t getLeastUnsetBitIndex() const
        {
            for (std::size_t wordIndex = 0; wordIndex < _wordsCount; ++wordIndex)
            {
                TWord word = _words[wordIndex];
                if (~word != 0) // If there's at least one unset bit
                {
                    for (std::size_t bit = 0; bit < _bitsPerWord; ++bit)
                    {
                        if ((word & (TWord(1) << bit)) == 0)
                        {
                            const std::size_t index = wordIndex * _bitsPerWord + bit;
                            return index < _bitsCount ? index : npos;
                        }
                    }
                }
            }
            return npos;
        }

        void clearAll()
        {
            for (std::size_t i = 0; i < _wordsCount; ++i)
            {
                _words[i] = 0;
            }
        }

        void setAll()
        {
            for (std::size_t i = 0; i < _wordsCount; ++i)
            {
                _words[i] = ~TWord(0);
            }
        }

        [[nodiscard]] std::size_t size() const
        {
            return _bitsCount;
        }
    };
}
