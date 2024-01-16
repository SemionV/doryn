#pragma once

#include "base/dependencies.h"
#include "base/concurrency/messaging_book.h"

namespace dory::testing::atm_book
{
    struct withdraw
    {
        std::string account;
        unsigned amount;
        mutable concurrency::messaging::sender atm_queue;
        withdraw(std::string const& account_,
                 unsigned amount_,
                 concurrency::messaging::sender atm_queue_):
                account(account_),amount(amount_),
                atm_queue(atm_queue_)
        {}
    };
    struct withdraw_ok
    {};
    struct withdraw_denied
    {};
    struct cancel_withdrawal
    {
        std::string account;
        unsigned amount;
        cancel_withdrawal(std::string const& account_,
                          unsigned amount_):
                account(account_),amount(amount_)
        {}
    };
    struct withdrawal_processed
    {
        std::string account;
        unsigned amount;
        withdrawal_processed(std::string const& account_,
                             unsigned amount_):
                account(account_),amount(amount_)
        {}
    };
    struct card_inserted
    {
        std::string account;
        explicit card_inserted(std::string const& account_):
                account(account_)
        {}
    };
    struct digit_pressed
    {
        char digit;
        explicit digit_pressed(char digit_):
                digit(digit_)
        {}
    };
    struct clear_last_pressed
    {};
    struct eject_card
    {};
    struct withdraw_pressed
    {
        unsigned amount;
        explicit withdraw_pressed(unsigned amount_):
                amount(amount_)
        {}
    };
    struct cancel_pressed
    {};
    struct issue_money
    {
        unsigned amount;
        issue_money(unsigned amount_):
                amount(amount_)
        {}
    };
    struct verify_pin
    {
        std::string account;
        std::string pin;
        mutable concurrency::messaging::sender atm_queue;
        verify_pin(std::string const& account_,std::string const& pin_,
                   concurrency::messaging::sender atm_queue_):
                account(account_),pin(pin_),atm_queue(atm_queue_)
        {}
    };
    struct pin_verified
    {};
    struct pin_incorrect
    {};
    struct display_enter_pin
    {};
    struct display_enter_card
    {};
    struct display_insufficient_funds
    {};
    struct display_withdrawal_cancelled
    {};
    struct display_pin_incorrect_message
    {};
    struct display_withdrawal_options
    {};
    struct get_balance
    {
        std::string account;
        mutable concurrency::messaging::sender atm_queue;
        get_balance(std::string const& account_, concurrency::messaging::sender atm_queue_):
                account(account_),atm_queue(atm_queue_)
        {}
    };
    struct balance
    {
        unsigned amount;
        explicit balance(unsigned amount_):
                amount(amount_)
        {}
    };
    struct display_balance
    {
        unsigned amount;
        explicit display_balance(unsigned amount_):
                amount(amount_)
        {}
    };
    struct balance_pressed
    {};

    class atm
    {
        concurrency::messaging::receiver incoming;
        concurrency::messaging::sender bank;
        concurrency::messaging::sender interface_hardware;
        void (atm::*state)();
        std::string account;
        unsigned withdrawal_amount;
        std::string pin;
        void process_withdrawal()
        {
            incoming.wait()
                    .handle<withdraw_ok>(
                            [&](withdraw_ok const& msg)
                            {
                                interface_hardware.send(
                                        issue_money(withdrawal_amount));
                                bank.send(
                                        withdrawal_processed(account,withdrawal_amount));
                                state=&atm::done_processing;
                            }
                    )
                    .handle<withdraw_denied>(
                            [&](withdraw_denied const& msg)
                            {
                                interface_hardware.send(display_insufficient_funds());
                                state=&atm::done_processing;
                            }
                    )
                    .handle<cancel_pressed>(
                            [&](cancel_pressed const& msg)
                            {
                                bank.send(
                                        cancel_withdrawal(account,withdrawal_amount));
                                interface_hardware.send(
                                        display_withdrawal_cancelled());
                                state=&atm::done_processing;
                            }
                    );
        }
        void process_balance()
        {
            incoming.wait()
                    .handle<balance>(
                            [&](balance const& msg)
                            {
                                interface_hardware.send(display_balance(msg.amount));
                                state=&atm::wait_for_action;
                            }
                    )
                    .handle<cancel_pressed>(
                            [&](cancel_pressed const& msg)
                            {
                                state=&atm::done_processing;
                            }
                    );
        }
        void wait_for_action()
        {
            interface_hardware.send(display_withdrawal_options());
            incoming.wait()
                    .handle<withdraw_pressed>(
                            [&](withdraw_pressed const& msg)
                            {
                                withdrawal_amount=msg.amount;
                                bank.send(withdraw(account,msg.amount,incoming));
                                state=&atm::process_withdrawal;
                            }
                    )
                    .handle<balance_pressed>(
                            [&](balance_pressed const& msg)
                            {
                                bank.send(get_balance(account,incoming));
                                state=&atm::process_balance;
                            }
                    )
                    .handle<cancel_pressed>(
                            [&](cancel_pressed const& msg)
                            {
                                state=&atm::done_processing;
                            }
                    );
        }
        void verifying_pin()
        {
            incoming.wait()
                    .handle<pin_verified>(
                            [&](pin_verified const& msg)
                            {
                                state=&atm::wait_for_action;
                            }
                    )
                    .handle<pin_incorrect>(
                            [&](pin_incorrect const& msg)
                            {
                                interface_hardware.send(
                                        display_pin_incorrect_message());
                                state=&atm::done_processing;
                            }
                    )
                    .handle<cancel_pressed>(
                            [&](cancel_pressed const& msg)
                            {
                                state=&atm::done_processing;
                            }
                    );
        }
        void getting_pin()
        {
            incoming.wait()
                    .handle<digit_pressed>(
                            [&](digit_pressed const& msg)
                            {
                                unsigned const pin_length=4;
                                pin+=msg.digit;
                                if(pin.length()==pin_length)
                                {
                                    bank.send(verify_pin(account,pin,incoming));
                                    state=&atm::verifying_pin;
                                }
                            }
                    )
                    .handle<clear_last_pressed>(
                            [&](clear_last_pressed const& msg)
                            {
                                if(!pin.empty())
                                {
                                    pin.pop_back();
                                }
                            }
                    )
                    .handle<cancel_pressed>(
                            [&](cancel_pressed const& msg)
                            {
                                state=&atm::done_processing;
                            }
                    );
        }
        void waiting_for_card()
        {
            interface_hardware.send(display_enter_card());
            incoming.wait()
                    .handle<card_inserted>(
                            [&](card_inserted const& msg)
                            {
                                account=msg.account;
                                pin="";
                                interface_hardware.send(display_enter_pin());
                                state=&atm::getting_pin;
                            }
                    );
        }
        void done_processing()
        {
            interface_hardware.send(eject_card());
            state=&atm::waiting_for_card;
        }
        atm(atm const&)=delete;
        atm& operator=(atm const&)=delete;
    public:
        atm(concurrency::messaging::sender bank_,
            concurrency::messaging::sender interface_hardware_):
                bank(bank_),interface_hardware(interface_hardware_)
        {}
        void done()
        {
            get_sender().send(concurrency::messaging::close_queue());
        }
        void run()
        {
            state=&atm::waiting_for_card;
            try
            {
                for(;;)
                {
                    (this->*state)();
                }
            }
            catch(concurrency::messaging::close_queue const&)
            {
            }
        }
        concurrency::messaging::sender get_sender()
        {
            return incoming;
        }
    };

    class bank_machine
    {
        concurrency::messaging::receiver incoming;
        unsigned balance;
    public:
        bank_machine():
                balance(199)
        {}
        void done()
        {
            get_sender().send(concurrency::messaging::close_queue());
        }
        void run()
        {
            try
            {
                for(;;)
                {
                    incoming.wait()
                            .handle<verify_pin>(
                                    [&](verify_pin const& msg)
                                    {
                                        if(msg.pin=="1234")
                                        {
                                            msg.atm_queue.send(pin_verified());
                                        }
                                        else
                                        {
                                            msg.atm_queue.send(pin_incorrect());
                                        }
                                    }
                            )
                            .handle<withdraw>(
                                    [&](withdraw const& msg)
                                    {
                                        if(balance>=msg.amount)
                                        {
                                            msg.atm_queue.send(withdraw_ok());
                                            balance-=msg.amount;
                                        }
                                        else
                                        {
                                            msg.atm_queue.send(withdraw_denied());
                                        }
                                    }
                            )
                            .handle<get_balance>(
                                    [&](get_balance const& msg)
                                    {
                                        msg.atm_queue.send(atm_book::balance(balance));
                                    }
                            )
                            .handle<withdrawal_processed>(
                                    [&](withdrawal_processed const& msg)
                                    {
                                    }
                            )
                            .handle<cancel_withdrawal>(
                                    [&](cancel_withdrawal const& msg)
                                    {
                                    }
                            );
                }
            }
            catch(concurrency::messaging::close_queue const&)
            {
            }
        }
        concurrency::messaging::sender get_sender()
        {
            return incoming;
        }
    };

    class interface_machine
    {
        concurrency::messaging::receiver incoming;
        std::mutex iom;
    public:
        void done()
        {
            get_sender().send(concurrency::messaging::close_queue());
        }
        void run()
        {
            try
            {
                for(;;)
                {
                    incoming.wait()
                            .handle<issue_money>(
                                    [&](issue_money const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"Issuing "
                                                     <<msg.amount<<std::endl;
                                        }
                                    }
                            )
                            .handle<display_insufficient_funds>(
                                    [&](display_insufficient_funds const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"Insufficient funds"<<std::endl;
                                        }
                                    }
                            )
                            .handle<display_enter_pin>(
                                    [&](display_enter_pin const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout
                                                    <<"Please enter your PIN (0-9)"
                                                    <<std::endl;
                                        }
                                    }
                            )
                            .handle<display_enter_card>(
                                    [&](display_enter_card const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"Please enter your card (I)"
                                                     <<std::endl;
                                        }
                                    }
                            )
                            .handle<display_balance>(
                                    [&](display_balance const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout
                                                    <<"The balance of your account is "
                                                    <<msg.amount<<std::endl;
                                        }
                                    }
                            )
                            .handle<display_withdrawal_options>(
                                    [&](display_withdrawal_options const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"Withdraw 50? (w)"<<std::endl;
                                            std::cout<<"Display Balance? (b)"
                                                     <<std::endl;
                                            std::cout<<"Cancel? (c)"<<std::endl;
                                        }
                                    }
                            )
                            .handle<display_withdrawal_cancelled>(
                                    [&](display_withdrawal_cancelled const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"Withdrawal cancelled"
                                                     <<std::endl;
                                        }
                                    }
                            )
                            .handle<display_pin_incorrect_message>(
                                    [&](display_pin_incorrect_message const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"PIN incorrect"<<std::endl;
                                        }
                                    }
                            )
                            .handle<eject_card>(
                                    [&](eject_card const& msg)
                                    {
                                        {
                                            std::lock_guard<std::mutex> lk(iom);
                                            std::cout<<"Ejecting card"<<std::endl;
                                        }
                                    }
                            );
                }
            }
            catch(concurrency::messaging::close_queue&)
            {
            }
        }
        concurrency::messaging::sender get_sender()
        {
            return incoming;
        }
    };
}