#include <iostream>
#include <utility>

#include "include/common.h"

#include "bms/bms.h"
#include "dms/dms.h"
#include "bms/Replacer.h"

BufferFrame bufferpool[DEFAULT_BUFFERSIZE]{};

unsigned int myhash(int seed) 
{
    return std::hash<int>{}(seed)%DEFAULT_BUFFERSIZE;
}


int main()
{

    dms d;
    d.OpenFile("data.dbf");
    bms b("mybms",&d,std::function<int(int)>{myhash});

    auto dir = d.GetDirView();

    int new_frame_id = b.FixNewPage();
    std::cout << new_frame_id << std::endl;
    std::cout << b.GetFreeFrameNumber() << std::endl;


    int new_frame_id_2 = b.FixPage(0);
    std::cout << "check page_id 0 frame_id : " << new_frame_id_2 << std::endl;
    assert(0==b.FrameIdtoPageId(new_frame_id_2));





    return 0;
}

