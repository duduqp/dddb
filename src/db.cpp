#include <iostream>
#include "include/common.h"

#include "bms/bms.h"
#include "dms/dms.h"


BufferFrame bufferpool[DEFAULT_BUFFERSIZE]{};


int main()
{

    dms d;
    d.OpenFile("data.dbf");
    std::cout << "Hello world" << std::endl;
    return 0;
}

