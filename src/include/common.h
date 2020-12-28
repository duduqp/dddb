#pragma once
#include <functional>
#include <queue>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <list>
#include <bitset>
#include <cmath>
#include <map>


#include <cassert>
const int FRAMESIZE = 4096;
const int DEFAULT_BUFFERSIZE=1024;// 1024 FRAMES
const int MAXPAGE = 50000;

struct BufferFrame{
    BufferFrame()
    {
        std::fill(field,field+FRAMESIZE,0);
    }
    char field[FRAMESIZE];
};


struct BlockControlInfo{
    BlockControlInfo()=default;
    BlockControlInfo(int p_page_id,int p_frame_id):
        pin_count(0),dirty(false),page_id(p_page_id),frame_id(p_frame_id){}

    int pin_count;
    bool dirty;
    int page_id;
    int frame_id;
};



