#include "bms.h"
#include "../include/common.h"
void bms::LRU_Replacer::AddCandidate(ptr_bc cand)
{
    m_candidate.push_front(cand);
}

int bms::LRU_Replacer::Evict()
{
    int frame_id_ret = -1;
    if(m_candidate.empty()) return frame_id_ret;
    ptr_bc out = m_candidate.back();
    frame_id_ret=out->frame_id;
    m_free.push_back(out);
    m_candidate.pop_back();
    return frame_id_ret;
}


void bms::LRU_Replacer::LiftUp(ptr_bc bc)
{
    auto it=std::find(m_candidate.begin(),m_candidate.end(),bc);
    ptr_bc to_swap(*it);
    m_candidate.push_front(to_swap);
    m_candidate.erase(it);
}

int bms::FixPage(int p_page_id,int p_protection){
    //if current page inbuffer
    auto bucket = m_allocatedframe[m_hashfunc(p_page_id)];
    if(bucket.empty()) return -1;

    auto it = bucket.cbegin();
    for(;it!=bucket.cend();++it)
    {
        if((*it)->page_id==p_page_id)
        {
            break;
        }
    }
    //inbuffer
    if(it!=bucket.cend()){
        return (*it)->frame_id;
    }

    //allocate new frame
    
    //if buffer full
    if(m_freeframenumber==0)
    {
        std::cout << "buffer pool already full" << "\n";
        //evict 
        int to_evict = m_replacer.Evict();

        //insert into bucket
        m_dms.Read
    }


}
    int FixNewPage();
    int UnFixPage(int p_page_id);
    int SetDirty(int p_page_id);
    int UnSetDirty(int p_page_id);
    int GetFreeFrameNumber() const ;
    void FlushBack();

    // internal
    void evict();



