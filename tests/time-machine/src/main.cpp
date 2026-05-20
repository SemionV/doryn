#include <iostream>
#include <memory>

int main()
{
    std::cout << "Hello World!" << std::endl;

    auto loc1 = std::make_unique<int>();
    int var1 = 1;
    *loc1 = var1;
}