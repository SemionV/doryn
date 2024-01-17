#include "dependencies.h"
#include "base/concurrency/log.h"
#include "base/testing/atm_book.h"


using namespace dory::testing::atm_book;

TEST_CASE( "run atm", "[atm_book]" )
{
    using Log = dory::concurrency::Log<decltype(std::cout)>;
    auto log = Log(std::cout);

    bank_machine bank(log);
    interface_machine interface_hardware(log);
    atm machine(log, bank.get_sender(),interface_hardware.get_sender());
    std::thread bank_thread(&bank_machine<Log>::run,&bank);
    std::thread if_thread(&interface_machine<Log>::run,&interface_hardware);
    std::thread atm_thread(&atm<Log>::run,&machine);
    dory::concurrency::messaging::sender atmqueue(machine.get_sender());
    bool quit_pressed=false;
    while(!quit_pressed)
    {
        char c=getchar();
        switch(c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                atmqueue.send(digit_pressed(c));
                break;
            case 'b':
                atmqueue.send(balance_pressed());
                break;
            case 'w':
                atmqueue.send(withdraw_pressed(50));
                break;
            case 'c':
                atmqueue.send(cancel_pressed());
                break;
            case 'q':
                quit_pressed=true;
                break;
            case 'i':
                atmqueue.send(card_inserted("acc1234"));
                break;
        }
    }
    bank.done();
    machine.done();
    interface_hardware.done();
    atm_thread.join();
    bank_thread.join();
    if_thread.join();
}