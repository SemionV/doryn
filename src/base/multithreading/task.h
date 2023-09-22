#pragma once

#include "base/doryExport.h"

namespace dory::multithreading
{
    class DORY_API Task
    {
        private:
            bool isDoneFlag;
            bool isErrorFlag;

        protected:
            void setDone(bool isDone);
            void setError(bool isError);
            virtual void invoke() = 0;

        public:
            Task();

            bool getDone();
            bool getError();
            void reset();
            virtual void operator()();
    };

    /*
    * For details see https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments
    */
    template <typename... Ts>
    class ActionTask: public Task
    {
        private:
            std::function<void (Ts...)> function;
            std::tuple<Ts...> arguments;

        protected:
            virtual void invoke() override
            {    
                std::apply(function, arguments);            
            }

        public:
            template <typename F>
            ActionTask(F&& function, Ts&&... arguments):
                function(std::forward<F>(function)),
                arguments(std::forward<Ts>(arguments)...)
            {
            }
    };

    template <typename TResult = void, typename... Ts>
    class FunctionTask: public Task
    {
        private:
            std::function<TResult (Ts...)> function;
            std::tuple<Ts...> arguments;
            TResult result;

        protected:
            virtual void invoke() override
            {          
                result = std::apply(function, arguments);
            }

        public:
            template <typename F>
            FunctionTask(F&& function, Ts&&... arguments):
                function(std::forward<F>(function)),
                arguments(std::forward<Ts>(arguments)...)
            {
            }

            TResult getResult(){
                return result;
            }
    };

    template <class TResult, typename F, typename... Args>
    FunctionTask<TResult, Args...> makeFunctionTask(F&& f, Args&&... args)
    {
        return FunctionTask<TResult, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <class TResult, typename F, typename... Args>
    std::shared_ptr<FunctionTask<TResult, Args...>> allocateFunctionTask(F&& f, Args&&... args)
    {
        return std::make_shared<FunctionTask<TResult, Args...>>(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    ActionTask<Args...> makeActionTask(F&& f, Args&&... args)
    {
        return ActionTask<Args...>(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    std::shared_ptr<ActionTask<Args...>> allocateActionTask(F&& f, Args&&... args)
    {
        return std::make_shared<ActionTask<Args...>>(std::forward<F>(f), std::forward<Args>(args)...);
    }
}