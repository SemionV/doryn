#pragma once

namespace dory
{
    /*template<typename RetVal, typename... Args>
    class IDelegate
    {
        public:
            virtual RetVal Execute(Args&&... args) = 0;
    };

    template<typename TLambda, typename RetVal, typename... Args>
    class LambdaDelegate : public IDelegate<RetVal, Args...>
    {
        public:
            explicit LambdaDelegate(TLambda&& lambda): 
                m_Lambda(std::forward<TLambda>(lambda))
            {                
            }

            RetVal Execute(Args&&... args) override
            {
                return (RetVal)((m_Lambda)(std::forward<Args>(args)...);
            }
        private:
            TLambda m_Lambda;
    };

    class Task
    {
        private:
            bool isDoneFlag;
            bool isErrorFlag;

        public:
            Task::Task():
                isDoneFlag(false)
            {                
            }

            void setDone(bool isDone)
            {
                isDoneFlag = isDone;
            }

            bool getDone()
            {
                return isDoneFlag;
            }

            void setError(bool isError)
            {
                isErrorFlag = isError;
            }

            bool getError()
            {
                return isErrorFlag;
            }

            virtual void operator()() = 0;
    };
    
    class DORY_API SystemThread
    {
        private:
            Task* regularTask;
            std::vector<Task&> irregularTasks;
            std::mutex mutex;
            bool isStop;

        public:
            explicit SystemThread(Task* regularTask):
                regularTask(regularTask)
            {
            }

            explicit SystemThread():
                regularTask(nullptr)
            {
            }

            virtual void invokeTask(Task& task)
            {
                task.setDone(false);
                task.setError(false);

                mutex.lock();
                irregularTasks.push_back(task);
                mutex.unlock();

                while(!task.getDone());

                mutex.lock();
                std::size_t count = irregularTasks.size();
                for(std::size_t i = 0; i < count; i++)
                {
                    if(&irregularTasks[i] == &task)
                    {
                        irregularTasks.erase(irregularTasks.begin() + i);
                        break;
                    }
                }
                mutex.unlock();
            }

            virtual void stop()
            {
                isStop = true;
            }

            virtual void run()
            {
                std::thread workingThread = std::thread(&threadMain, this);
                workingThread.detach();
            }

        private:
            virtual void threadMain()
            {
                while(!isStop)
                {
                    std::size_t count = irregularTasks.size();
                    for(std::size_t i = 0; i < count; i++)
                    {
                        Task& task = irregularTasks[i];
                        try
                        {
                            task();
                        }
                        catch(const std::exception& e)
                        {
                            task.setError(true);
                        }
                        
                        task.setDone(true);
                    }

                    if(regularTask)
                    {
                        regularTask->operator()();
                    }

                    //const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(5);
                    //std::this_thread::sleep_for(threadMainSleepInterval);
                }
            }
    };*/
}