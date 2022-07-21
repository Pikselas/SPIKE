#include <iostream>
#include"NetworkChannel.h"
#include<vector>

int main()
{
    std::vector<char> v;
 
    for (auto i = 0 ; i < v.size() ; ++i)
    {
        std::cout << v.data()[i];
    }
    return 0;
}
