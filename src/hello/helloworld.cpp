#include <iostream>
#include <vector>
#include <string>
#include "test/test.h"
#include "test/test2/test2.h"
#include "../mathLibrary/math.h"
#include "../messageLibrary/message.h"

using namespace std;

int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;

#ifdef TEST
    test();
#endif

#ifdef TEST2
    test2();
#endif

    cout << "sum 2+2:" << sum(2, 2) << endl;

    message("Message lib test");

    return 0;
}