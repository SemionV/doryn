namespace dory::testing
{
    template<typename T>
    class lock_free_stack
    {
    private:
        struct node
        {
            std::shared_ptr<T> data;
            node* next;
            node(T const& data_):
                data(std::make_shared<T>(data_))
            {}
        };
        std::atomic<node*> head;
        std::atomic<unsigned> threads_in_pop;
        std::atomic<node*> to_be_deleted;

        static void delete_nodes(node* nodes)
        {
            while(nodes)
            {
                node* next=nodes->next;
                delete nodes;
                nodes=next;
            }
        }
        void try_reclaim(node* old_head)
        {
            if(threads_in_pop==1)
            {
                node* nodes_to_delete=to_be_deleted.exchange(nullptr);
                if(!--threads_in_pop)
                {
                    delete_nodes(nodes_to_delete);
                }
                else if(nodes_to_delete)
                {
                    chain_pending_nodes(nodes_to_delete);
                }
                delete old_head;
            }
            else
            {
                chain_pending_node(old_head);
                --threads_in_pop;
            }
        }
        void chain_pending_nodes(node* nodes)
        {
            node* last=nodes;
            while(node* const next=last->next)
            {
                last=next;
            }
            chain_pending_nodes(nodes,last);
        }
        void chain_pending_nodes(node* first,node* last)
        {
            last->next=to_be_deleted;
            while(!to_be_deleted.compare_exchange_weak(
                    last->next,first));
        }
        void chain_pending_node(node* n)
        {
            chain_pending_nodes(n,n);
        }

    public:
        void push(T const& data)
        {
            node* const new_node=new node(data);
            new_node->next=head.load();
            while(!head.compare_exchange_weak(new_node->next,new_node));
        }

        std::shared_ptr<T> pop()
        {
            ++threads_in_pop;
            node* old_head=head.load();
            while(old_head &&
                  !head.compare_exchange_weak(old_head,old_head->next));
            std::shared_ptr<T> res;
            if(old_head)
            {
                res.swap(old_head->data);
            }
            try_reclaim(old_head);
            return res;
        }
    };

    template<typename T>
    class lock_free_stack_4
    {
    private:
        struct node;

        struct counted_node_ptr
        {
            int external_count;
            node* ptr;
        };

        struct node
        {
            std::shared_ptr<T> data;
            std::atomic<int> internal_count;
            counted_node_ptr next;
            explicit node(T const& data_):
                    data(std::make_shared<T>(data_)),
                    internal_count(0)
            {}
        };

        std::atomic<counted_node_ptr> head;

        void increase_head_count(counted_node_ptr& old_counter)
        {
            counted_node_ptr new_counter;
            do
            {
                new_counter=old_counter;
                ++new_counter.external_count;
            }
            while(!head.compare_exchange_strong(old_counter,new_counter));
            old_counter.external_count=new_counter.external_count;
        }

    public:
        ~lock_free_stack_4()
        {
            while(pop());
        }

        void push(T const& data)
        {
            counted_node_ptr new_node;
            new_node.ptr=new node(data);
            new_node.external_count=1;
            new_node.ptr->next=head.load();
            while(!head.compare_exchange_weak(new_node.ptr->next,new_node));
        }

        std::shared_ptr<T> pop()
        {
            counted_node_ptr old_head=head.load();
            for(;;)
            {
                increase_head_count(old_head);
                node* const ptr=old_head.ptr;
                if(!ptr)
                {
                    return std::shared_ptr<T>();
                }
                if(head.compare_exchange_strong(old_head,ptr->next))
                {
                    std::shared_ptr<T> res;
                    res.swap(ptr->data);
                    int const count_increase=old_head.external_count-2;
                    if(ptr->internal_count.fetch_add(count_increase)==
                       -count_increase)
                    {
                        delete ptr;
                    }
                    return res;
                }
                else if(ptr->internal_count.fetch_sub(1)==1)
                {
                    delete ptr;
                }
            }
        }
    };
}