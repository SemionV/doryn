#include <iostream>
#include <vector>
#include <string>
#include "test.h"
#include "../mathLibrary/math.h"

using namespace std;

int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;

    test();

    cout << "sum 2+2:" << sum(2, 2);

    return 0;
}