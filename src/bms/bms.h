#pragma once
#include "../include/common.h"
#include "Replacer.h"
class dms;

class LRU_Replacer : public Replacer
{
public:
    typedef std::shared_ptr<BlockControlInfo> ptr_bc;
public:
    explicit LRU_Replacer(std::deque<ptr_bc> & p_free):m_free(p_free){}
    void AddCandidate(ptr_bc cand);
    void Evict(ptr_bc bc);
    void LiftUp(ptr_bc bc);
    ~LRU_Replacer(){  }    
private:
    std::list<ptr_bc> m_candidate;
    std::deque<ptr_bc> & m_free;
};



class bms
{
public:
    typedef const dms *  ptr_dms;
    typedef std::shared_ptr<BlockControlInfo> bc_bucket;
public:
    bms(std::string p_name,const dms * p_dms,std::function<int(int)> p_hashfunc):bms_name(p_name),m_freeframenumber(0),
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
    int SetDirty(int p_page_id);
    int UnSetDirty(int p_page_id);
    int GetFreeFrameNumber() const ;
    void FlushBack();

    // internal
    void evict();
    

    ~bms() {
        //FLUSH BACK BUFFER
        FlushBack();
    }

private:
    class LRU_Replacer : public Replacer
    {
    public:
        typedef std::shared_ptr<BlockControlInfo> ptr_bc;
    public:
        explicit LRU_Replacer(std::deque<ptr_bc> & p_free):m_free(p_free){}
        void AddCandidate(ptr_bc cand);
        int  Evict();
        void LiftUp(ptr_bc bc);

    private:
        std::list<ptr_bc> m_candidate;
        std::deque<ptr_bc> & m_free;
    };


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
    std::function<int(int)> m_hashfunc;
    LRU_Replacer m_replacer; 
};

