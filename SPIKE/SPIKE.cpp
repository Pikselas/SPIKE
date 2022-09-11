#include <iostream>
#include<iterator>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer("3456");
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
