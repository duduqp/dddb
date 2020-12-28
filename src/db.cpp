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

    /*
    int new_frame_id = b.FixPage(0);
    int new_frame_id_0=new_frame_id;
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


    int new_frame_id_0_rep = b.FixPage(0);
    assert(new_frame_id_0_rep==new_frame_id_0);
    auto ret = b.Evict();
    std::cout << "Show evicted frame : "<<std::endl;
    b.ShowFrameContent(ret->frame_id);


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
    
*/

    
    std::ifstream ifs("data-5w-50w-zipf");
    char test_line[10];
    std::fill(test_line,test_line+10,0);
    int i = 0;
    int p_page_id ;
    while(ifs.getline(test_line,sizeof(test_line),'\n'))
    {
        std::cout << " test case " <<i<<":"<<test_line<<std::endl;
        int p_page_id = atoi(&test_line[2]);
        if(test_line[0]=='0')
        {
            b.FixPage(p_page_id);
        }else{
            b.FixPage(p_page_id);
            b.SetDirty(p_page_id,true);
        }
        i++;
    }
    std::cout<< "total test case : " <<i<<std::endl;

    return 0;
}

