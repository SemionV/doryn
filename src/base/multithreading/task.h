#pragma once

#include "base/doryExport.h"

namespace dory
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

    template<class TResult, typename... Args>
    class DORY_API TaskFunction: Task
    {
        private:
            TResult result;
            std::tuple<Args...> args;

        protected:
            virtual void invoke() override
            {                
            }

            virtual TResult invoke(Args... args) = 0;

        public:
            TaskFunction(Args&&... args):
                args(std::forward<Args>(args)...)
            {

            }

            TResult getResult(){
                return result;
            }

            virtual void operator()() override
            {
                try
                {
                    result = invoke(args);
                    setDone(true);
                }
                catch(...)
                {
                    setError(true);
                }
            }
    };

    namespace helper
    {
        template <std::size_t... Ts>
        struct index 
        {
            enum { count = sizeof...(Ts) };
            std::size_t values[count] = {Ts...};
            const std::size_t valuesCount = count;
        };
    
        template <std::size_t N, std::size_t... Ts>
        struct gen_seq : gen_seq<N - 1, N - 1, Ts...> {};
    
        template <std::size_t... Ts>
        struct gen_seq<0, Ts...> : index<Ts...> {};
    }
    
    template <class TResult, typename... Ts>
    class Action_impl
    {
        private:
            std::function<TResult (Ts...)> f;
            std::tuple<Ts...> args;
        public:
            TResult result;

            template <typename F>
            Action_impl(F&& func, Ts&&... args)
                : f(std::forward<F>(func)),
                args(std::forward<Ts>(args)...)
            {}
    
            template <typename... Args, std::size_t... Is>
            TResult func(std::tuple<Args...>& tup, helper::index<Is...>)
            {
                return f(std::get<Is>(tup)...);
            }
    
            template <typename... Args>
            TResult func(std::tuple<Args...>& tup)
            {
                return func(tup, helper::gen_seq<sizeof...(Args)>{});
            }
    
            void act()
            {
                result = func(args);
            }
    };
    
    template <class TResult, typename F, typename... Args>
    Action_impl<TResult, Args...> make_action(F&& f, Args&&... args)
    {
        return Action_impl<TResult, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
    }
}