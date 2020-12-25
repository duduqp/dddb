#include "bms.h"
#include "../include/common.h"
#include "../dms/dms.h"
void LRU_Replacer::AddCandidate(ptr_bc cand)
{
    int frame_id_key = cand->frame_id;
    std::shared_ptr<LRU_Node> to_add(std::make_shared<LRU_Node>(m_candidate_header,m_candidate_header->next,cand));
    m_candidate_header->next=to_add;
    if(!m_candidate_header->prev) m_candidate_header->prev=to_add;
     m_candidate.insert({frame_id_key,to_add});
    
}

LRU_Replacer::ptr_bc LRU_Replacer::Evict()
{
    if(!m_candidate_header->prev) return nullptr;
    auto to_out = m_candidate_header->prev;
    to_out->prev->next=nullptr;
    m_candidate_header->prev=to_out->prev;
    m_candidate.erase(to_out->BCI->frame_id);
    m_free.push_back(to_out->BCI);
    return to_out->BCI;
}


void LRU_Replacer::LiftUp(int frame_id)
{
    if(0==m_candidate.count(frame_id)) return ;

    auto pos=m_candidate[frame_id];
    AddCandidate(pos->BCI);

    pos->prev->next=pos->next;
    pos->next->prev=pos->prev;

    assert(pos.use_count()==0);
    m_candidate.erase(frame_id);
    m_candidate.insert({frame_id,m_candidate_header->next});
}
void LRU_Replacer::Remove(int frame_id)
{
   if(0==m_candidate.count(frame_id))
   {
       std::cerr << "frame_id : "<<frame_id<<" Not In LRU"<<std::endl;
   }else{
        auto target = m_candidate[frame_id];
        target->prev->next=target->next;
        target->next->prev=target->prev;
        m_free.push_back(target->BCI);
        m_candidate.erase(frame_id);
        std::cout << "LRU Remove : " <<frame_id <<std::endl;
   }
}

int bms::FixPage(int p_page_id,int p_protection){
    //if current page inbuffer
    auto bucket = m_allocatedframe[m_hashfunc(p_page_id)];

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
        //lift it to most recently used end
        m_replacer.LiftUp((*it)->frame_id);

        //hit
        //...
        
        return (*it)->frame_id;
    }

    //allocate new frame
    //miss -> execute some io
    //...
    
    //if buffer full
    if(0==m_freeframenumber)
    {
        std::cerr << "buffer pool already full" << "\n";
        //evict 
        bc_bucket ret = m_replacer.Evict();

        if(ret)
        {
            if(ret->dirty)
            {
                //write back
                m_dms->WritePage(ret->page_id,ret);
                ret->dirty=false;
            }
        }else{
            //no free frame and can not evict?
            std::cerr<<"No free frame and Evict Fail!\n";
            return -1;
        }
        ret->page_id = -1;
    }
     //now buffer is available
     //pick a bcb from freeframe
    auto free_frame = m_freeframe.front();
    m_freeframe.pop_front();


    //if it is a new page dms will return imediately
    //else readpage
    m_dms->ReadPage(p_page_id,free_frame);
    free_frame->page_id=p_page_id;
    bucket.push_back(free_frame);

    //add into lru
    m_replacer.AddCandidate(free_frame);
    
    //update metadata
    m_freeframenumber--;
    m_frame2page[free_frame->frame_id]=p_page_id;
    return free_frame->frame_id;
}
int bms::FixNewPage()
{
    //always set dirty flag after call this method
    
    int new_page_id = m_dms->NewPage();
    int new_frame_id = FixPage(new_page_id);
    SetDirty(new_page_id,true);//new page always dirty
    return new_frame_id;
}

int bms::UnFixPage(int p_page_id)
{
    //find corresponding BCI
    auto ret= LocateBlockControlInfo(p_page_id);
    if(!ret)
    {
        std::cerr<<"Have not been in buffer yet!\n";
        return -1;
    }
    m_replacer.Remove(ret->frame_id);
    ret->page_id=-1;
    m_freeframe.push_back(ret);
    m_allocatedframe[m_hashfunc(p_page_id)].remove(ret);

}
int bms::SetDirty(int p_page_id,bool flag)
{
    auto ret= LocateBlockControlInfo(p_page_id);
    if(!ret) return -1;
    ret->dirty = flag;
    
    return 0;//0 for success
}

int bms::GetFreeFrameNumber() const{
    return m_freeframenumber;
}
void bms::FlushBack()
{
    //flush back all alocated frame and reset metadata 
    //may lock ...
    
    for(int i=0;i<DEFAULT_BUFFERSIZE;++i)
    {
        if(auto bucket= m_allocatedframe[i];!bucket.empty())
        {
            for(auto & bc: bucket)
            {
                if(bc->dirty) m_dms->WritePage(bc->page_id,bc);
                m_freeframe.push_back(bc);
                bucket.remove(bc);
                m_replacer.Remove(bc->frame_id);
                bc->dirty=false;
                bc->page_id=-1;
                //bc->pin_count=0;
            }
        }
    }

    std::fill(m_frame2page,m_frame2page+DEFAULT_BUFFERSIZE,-1);
    m_freeframenumber=DEFAULT_BUFFERSIZE;

}

bms::bc_bucket bms::LocateBlockControlInfo(int p_page_id)
{
    auto bucket = m_allocatedframe[m_hashfunc(p_page_id)];

    auto it = bucket.cbegin();
    for(;it!=bucket.cend();++it)
    {
        if((*it)->page_id==p_page_id)
        {
            break;
        }
    }
    //inbuffer
    if(it!=bucket.cend())
        return *it;
    else
        return nullptr;
}


