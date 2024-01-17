#pragma once

#include "base/dependencies.h"

namespace dory::concurrency::messaging
{
    struct message_base
    {
        virtual ~message_base()
        {}
    };

    template<typename Msg>
    struct wrapped_message:
            message_base
    {
        Msg contents;
        explicit wrapped_message(Msg const& contents_):
                contents(contents_)
        {}
    };

    template<typename TLog>
    class queue
    {
        std::mutex m;
        std::condition_variable c;
        std::queue<std::shared_ptr<message_base> > q;
        TLog& log;
    public:
        explicit queue(TLog& log):
            log(log)
        {}

        template<typename T>
        void push(T const& msg)
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::make_shared<wrapped_message<T> >(msg));
            c.notify_all();
        }
        std::shared_ptr<message_base> wait_and_pop()
        {
            std::unique_lock<std::mutex> lk(m);
            c.wait(lk,[&]{return !q.empty();});
            auto res=q.front();
            q.pop();
            return res;
        }
    };

    template<typename TLog, int RecieverType, typename PreviousDispatcher,typename Msg,typename Func>
    class TemplateDispatcher
    {
        queue<TLog>* q;
        PreviousDispatcher* prev;
        Func f;
        bool chained;
        TLog& log;

        TemplateDispatcher(TemplateDispatcher const&)=delete;
        TemplateDispatcher& operator=(TemplateDispatcher const&)=delete;

        template<typename Log, int RType, typename Dispatcher,typename OtherMsg,typename OtherFunc>
        friend class TemplateDispatcher;

        void wait_and_dispatch()
        {
            log.printLine("wait_and_dispatch(", RecieverType, "): ", typeid(Msg).name());
            for(;;)
            {
                auto msg=q->wait_and_pop();
                if(dispatch(msg))
                    break;
            }
        }

        bool dispatch(std::shared_ptr<message_base> const& msg)
        {
            if(wrapped_message<Msg>* wrapper=
                    dynamic_cast<wrapped_message<Msg>*>(msg.get()))
            {
                f(wrapper->contents);
                return true;
            }
            else
            {
                return prev->dispatch(msg);
            }
        }
    public:
        TemplateDispatcher(TemplateDispatcher&& other):
                q(other.q),prev(other.prev),f(std::move(other.f)),
                chained(other.chained), log(other.log)
        {
            other.chained=true;
        }

        TemplateDispatcher(TLog& log, queue<TLog>* q_,PreviousDispatcher* prev_,Func&& f_):
                log(log), q(q_),prev(prev_),f(std::forward<Func>(f_)),chained(false)
        {
            prev_->chained=true;
        }

        template<typename OtherMsg, typename OtherFunc>
        TemplateDispatcher<TLog, RecieverType, TemplateDispatcher, OtherMsg, OtherFunc>
        handle(OtherFunc&& of)
        {
            return TemplateDispatcher<TLog, RecieverType,
                    TemplateDispatcher,OtherMsg,OtherFunc>(
                    log, q,this,std::forward<OtherFunc>(of));
        }

        ~TemplateDispatcher() noexcept(false)
        {
            if(!chained)
            {
                wait_and_dispatch();
            }
        }
    };

    class close_queue
    {};

    template<typename TLog, int RecieverType>
    class dispatcher
    {
        queue<TLog>* q;
        bool chained;
        TLog& log;

        dispatcher(dispatcher const&)=delete;
        dispatcher& operator=(dispatcher const&)=delete;

        template<typename Log,
                int RType,
                typename Dispatcher,
                typename Msg,
                typename Func>
        friend class TemplateDispatcher;

        void wait_and_dispatch()
        {
            log.printLine("wait_and_dispatch(", RecieverType, "): ", "root");
            for(;;)
            {
                auto msg=q->wait_and_pop();
                dispatch(msg);
            }
        }

        bool dispatch(
                std::shared_ptr<message_base> const& msg)
        {
            if(dynamic_cast<wrapped_message<close_queue>*>(msg.get()))
            {
                throw close_queue();
            }
            return false;
        }
    public:
        dispatcher(TLog& log, dispatcher&& other):
                q(other.q),chained(other.chained), log(log)
        {
            other.chained=true;
        }

        explicit dispatcher(TLog& log, queue<TLog>* q_):
                q(q_),chained(false), log(log)
        {}

        template<typename Message,typename Func>
        TemplateDispatcher<TLog, RecieverType, dispatcher,Message,Func>
        handle(Func&& f)
        {
            return TemplateDispatcher<TLog, RecieverType, dispatcher,Message,Func>(
                    log, q,this,std::forward<Func>(f));
        }

        ~dispatcher() noexcept(false)
        {
            if(!chained)
            {
                wait_and_dispatch();
            }
        }
    };

    template<typename TLog>
    class sender
    {
        queue<TLog>*q;
        TLog& log;
    public:
        sender(TLog& log):
                log(log), q(nullptr)
        {}
        explicit sender(TLog& log, queue<TLog>*q_):
                log(log), q(q_)
        {}
        template<typename Message>
        void send(Message const& msg)
        {
            if(q)
            {
                q->push(msg);
            }
        }
    };

    template<typename TLog, int RecieverType>
    class receiver
    {
        queue<TLog> q;
        TLog& log;
    public:
        receiver(TLog& log):
            q(log), log(log)
        {
        }

        operator sender<TLog>()
        {
            return sender(log, &q);
        }
        dispatcher<TLog, RecieverType> wait()
        {
            return dispatcher<TLog, RecieverType>(log, &q);
        }
    };
}