#include <iostream>
#include "../bms/bms.h"
#include "../include/common.h"


int main()
{
    //test for LRU_Replacer
    {
        std::deque<std::shared_ptr<BlockControlInfo>> freeframes;
        freeframes.push_back(std::move(std::make_shared<BlockControlInfo>(0,0)));
        freeframes.push_back(std::move(std::make_shared<BlockControlInfo>(1,1)));
        freeframes.push_back(std::move(std::make_shared<BlockControlInfo>(8,2)));
        freeframes.push_back(std::move(std::make_shared<BlockControlInfo>(7,3)));

        std::cout << freeframes[0].use_count() << std::endl;
        //LRU_Replacer
        LRU_Replacer lru{freeframes};
    }
    return 0;
}

