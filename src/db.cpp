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


    /*
    int new_frame_id = b.FixNewPage();
    std::cout << new_frame_id << std::endl;
    std::cout << b.GetFreeFrameNumber() << std::endl;
    

    auto dir=d.GetDirView();
    std::cout << "after fix new page  pagecount is "<<dir.GetPageCount()<<"page_max is "<<dir.GetPageMax()<<std::endl;
    */

    int new_frame_id = b.FixPage(0);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    assert(0==b.FrameIdtoPageId(new_frame_id));
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    int new_frame_id_10=b.FixPage(10);
    b.SetDirty(10,true);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    new_frame_id=b.FixPage(1000);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    int new_frame_id_20=b.FixPage(20);
    b.SetDirty(20,true);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);
    new_frame_id=b.FixPage(2);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    new_frame_id=b.FixPage(222);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    int new_frame_id_200=b.FixPage(200);
    b.SetDirty(200,true);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    new_frame_id=b.FixPage(1001);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    new_frame_id=b.FixPage(1013);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);

    new_frame_id=b.FixPage(408);
    std::cout << "check page_id 0 frame_id : " << new_frame_id << std::endl;
    std::cout << "Show frame_id " << new_frame_id <<std::endl;
    b.ShowFrameContent(new_frame_id);


    auto & lruview= b.GetLRU_Replacer();
    lruview.ShowLRUNodeInfo(new_frame_id_10);




    return 0;
}

