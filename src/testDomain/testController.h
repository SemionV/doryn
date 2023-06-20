#pragma once

#include <iostream>

using namespace std;

#include "../base/doryExport.h"
#include "../base/controller.h"

namespace test
{
    class DORY_API TestController: public dory::Controller
    {
        private:
            int counter;


        public:

        TestController();

        void initialize(dory::DataContext& context);

        void stop(dory::DataContext& context);

        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);
    };
}