#pragma once

#include "base/doryExport.h"
#include "message.h"

namespace dory
{
    class DORY_API MessageFactory
    {
        public:
            template<class TMessage>
            std::unique_ptr<TMessage> BuildMessage();
    };
}