#pragma once

#include "base/doryExport.h"

namespace dory
{
    class DORY_API Task
    {
        private:
            bool isDoneFlag;
            bool isErrorFlag;

        public:
            Task();

            void setDone(bool isDone);
            bool getDone();
            void setError(bool isError);
            bool getError();

            virtual void operator()() = 0;
    };
}