#pragma once

namespace dory
{
    /*
     * This class is supposed to be used as a base class of a CRTP implementation chain
     * CRTP implementation chain is designed to bypass the need to use virtual methods in class hierarchy
     * and enable compile to inline implementation methods from derived classes into base class method implementation
     */
    template <typename TImplementation>
    class Base
    {
    protected:
        TImplementation& implRef() noexcept
        {
            return static_cast<TImplementation&>(*this);
        }
        const TImplementation& implRef() const noexcept
        {
            return static_cast<const TImplementation&>(*this);
        }
    };
}