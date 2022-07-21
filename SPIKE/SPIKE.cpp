#include <iostream>
#include<vector>

#include"NetworkServer.h"
int main()
{
    try
    {
        NetworkServer ns("1234");
        std::vector<char> vc(100, 'c');
       
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
