#include <iostream>
#include "include/common.h"

#include "bms/bms.h"
#include "dms/dms.h"
#include "bms/Replacer.h"

BufferFrame bufferpool[DEFAULT_BUFFERSIZE]{};


int main()
{

    dms d;
    d.OpenFile("data.dbf");
    bms b("mybms",&d,[](int seed)->int{
          return (seed*42)%DEFAULT_BUFFERSIZE;
          });

    auto dir = d.GetDirView();

    std::cout << "initial dir page count : " << dir.GetPageCount() <<std::endl;
   
    std::cout << std::boolalpha << dir.IsPageUsed(13) <<std::endl;

    dir.SetPage(13,1);

    dir.ShowPageMap(13);
    std::cout << std::boolalpha << dir.IsPageUsed(13) <<std::endl;

    return 0;
}

