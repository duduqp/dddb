#pragma once
#include "../include/common.h"
#include "Replacer.h"
#include "../dms/dms.h"
extern BufferFrame bufferpool[DEFAULT_BUFFERSIZE];

class dms;

class LRU_Replacer : public Replacer
{
public:
    typedef std::shared_ptr<BlockControlInfo> ptr_bc;
public:
    explicit LRU_Replacer(std::deque<ptr_bc> & p_free):m_free(p_free){
        m_candidate_header=std::make_shared<LRU_Node>(nullptr,nullptr,nullptr);
        m_candidate_header->next=m_candidate_header;
        m_candidate_header->prev=m_candidate_header;
        m_candidate.insert({-1,m_candidate_header});
    }
    void AddCandidate(ptr_bc cand);
    ptr_bc Evict();
    void LiftUp(int frame_id);
    void Remove(int frame_id);

    ~LRU_Replacer(){  }    
private:
    struct LRU_Node{
        LRU_Node(std::shared_ptr<LRU_Node> p_prev,std::shared_ptr<LRU_Node> p_next,ptr_bc p_BCI):
            prev(p_prev),next(p_next),BCI(p_BCI){

            }
        std::shared_ptr<LRU_Node> prev;
        std::shared_ptr<LRU_Node> next;
        ptr_bc BCI;
    };
    std::shared_ptr<LRU_Node> m_candidate_header;
    std::map<int,std::shared_ptr<LRU_Node>> m_candidate;
    std::deque<ptr_bc> & m_free;
};



class bms
{
public:
    typedef  dms *  ptr_dms;
    typedef std::shared_ptr<BlockControlInfo> bc_bucket;
public:
    bms(std::string p_name,dms * p_dms,std::function<int(int)> p_hashfunc):bms_name(p_name),m_freeframenumber(DEFAULT_BUFFERSIZE),
        m_dms(p_dms),
        m_freeframe(std::deque<bc_bucket>(DEFAULT_BUFFERSIZE)),
        m_replacer(m_freeframe),
        m_hashfunc(p_hashfunc)
    {
        for (int i = 0; i < DEFAULT_BUFFERSIZE; ++i) {
            m_freeframe[i]=std::make_shared<BlockControlInfo>(-1,i);
            m_frame2page[i]=-1;
        }    
    }
    int FixPage(int p_page_id,int p_protection=0);
    int FixNewPage();
    int UnFixPage(int p_page_id);
    int SetDirty(int p_page_id,bool flag);
    int GetFreeFrameNumber() const ;
    int PageIdtoFrameId(int p_page_id) 
    {
        auto ret=LocateBlockControlInfo(p_page_id);
        
        return ret->frame_id;

        
    }
    int FrameIdtoPageId(int p_frame_id) const
    {
        return m_frame2page[p_frame_id];
    }
    void FlushBack();
    bc_bucket LocateBlockControlInfo(int p_page_id);
    // internal
    bc_bucket Evict(){ return m_replacer.Evict(); }
    

    ~bms() {
        //FLUSH BACK BUFFER
        std::cout << "bms will destruct but before will flush frame back"<<std::endl;
        FlushBack();
        std::cout << "after flush back" << std::endl;
    }
 

private:
    //UNCOPYABLE
    bms(const bms & ) = delete;
    bms & operator=(const bms & ) = delete;
   
    //MEMBER
    std::string bms_name;

    int m_freeframenumber;
    int m_frame2page[DEFAULT_BUFFERSIZE];
    ptr_dms m_dms;
    std::deque<bc_bucket> m_freeframe;
    std::list<bc_bucket> m_allocatedframe[DEFAULT_BUFFERSIZE];
    LRU_Replacer m_replacer; 
    std::function<int(int)> m_hashfunc;
};

