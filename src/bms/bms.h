#pragma once
#include "../include/common.h"
#include "Replacer.h"
class dms;

class LRU_Replacer : public Replacer
{
    typedef std::shared_ptr<BlockControlInfo> ptr_bc;
public:
    void AddCandidate(ptr_bc cand);
    void PushToHead(ptr_bc bc);
    void PushToTail(ptr_bc bc);
    void Remove(ptr_bc bc);
    ptr_bc SelectVictim();
    
private:
    std::deque<ptr_bc> m_candidate;

};
class bms
{
    typedef std::shared_ptr<dms> ptr_dms;
    typedef std::shared_ptr<BlockControlInfo> bc_bucket;
    typedef std::unique_ptr<Replacer> replace_policy;
public:
    bms();
    int FixPage(int page_id,int protection);
    int FixNewPage();
    int UnFixPage(int page_id);
    int SetDirty(int page_id);
    int UnSetDirty(int page_id);
    int GetFreeFrameNumber() const ;
    void FlushBack();

    // internal
    void evict();
    

    ~bms() {
        //FLUSH BACK BUFFER
        FlushBack();
    }

private:
    //UNCOPYABLE
    bms(const bms & ) = delete;
    bms & operator=(const bms & ) = delete;
   
    //MEMBER
    int m_freeframenumber;
    int m_frame2page[DEFAULT_BUFFERSIZE];
    bc_bucket m_freeframe;
    bc_bucket m_allocatedframe[DEFAULT_BUFFERSIZE];
    std::function<int(int)> m_hashfunc;
    std::shared_ptr<Replacer> m_replacer; 
};

