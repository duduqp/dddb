#include "bms.h"
#include "../include/common.h"
#include "../dms/dms.h"

void LRU_Replacer::ShowLRUNodeInfo(int p_frame_id)
{
    if(0==m_candidate.count(p_frame_id))
    {
        std::cerr << "p_frame_id : " <<p_frame_id <<" not in LRU !" << std::endl;
        return ;
    }

    auto bci = m_candidate[p_frame_id]->BCI;
    std::cout << "frame_id : " << bci->frame_id << " dirty : " <<bci->dirty <<" page_id : " <<bci->page_id <<"\n";
    std::cout << "frame content : " << "\n";
    std::cout << bufferpool[bci->frame_id].field<<std::endl;
}


void LRU_Replacer::AddCandidate(ptr_bc cand)
{
    int frame_id_key = cand->frame_id;
    std::shared_ptr<LRU_Node> to_add(std::make_shared<LRU_Node>(m_candidate_header,m_candidate_header->next,cand));
    m_candidate_header->next->prev=to_add;
    
    m_candidate_header->next=to_add;
    m_candidate.insert({frame_id_key,to_add});
   
     std::cout << "LRU after insert at front ";
     std::cout << m_candidate.size() << std::endl;
     std::cout << "m_candidate_header prev and next : " <<m_candidate_header->prev->BCI->frame_id <<" " <<
         m_candidate_header->next->BCI->frame_id<<std::endl;
     if(m_candidate.size()>2) std::cout << "for m_candidate_header's prev , its prev : " <<m_candidate_header->prev->prev->BCI->frame_id <<std::endl;

     if(m_candidate.size()>2) std::cout << "for m_candidate_header's next , its next : " <<m_candidate_header->next->next->BCI->frame_id<<std::endl;





}

LRU_Replacer::ptr_bc LRU_Replacer::Evict()
{
    if(m_candidate_header->prev==m_candidate_header) return nullptr;
    auto to_out = m_candidate_header->prev;
    std::cout << "to out 's prev : " <<to_out->prev->BCI->frame_id<<std::endl;
    //! to out next is header , no BCI info !std::cout << "to out 's next : " <<to_out->next->BCI->frame_id<<std::endl;
    to_out->prev->next=m_candidate_header;
    m_candidate_header->prev=to_out->prev;
    m_candidate.erase(to_out->BCI->frame_id);
    m_free.push_back(to_out->BCI);
    std::cout << " Evict "<<to_out->BCI->frame_id<<std::endl;
    return to_out->BCI;
}


void LRU_Replacer::LiftUp(int frame_id)
{
    std::cout << "LRU lift up  frame_id " << frame_id << std::endl; 
    if(0==m_candidate.count(frame_id)) return ;

    
    auto &pos=m_candidate[frame_id];
    if(m_candidate_header->next==pos) return ;


    pos->prev->next=pos->next;
    pos->next->prev=pos->prev;

    pos->next=m_candidate_header->next;
    pos->prev=m_candidate_header;

    m_candidate_header->next->prev=pos;
    
    m_candidate_header->next=pos;
    std::cout << "adjust lru list ptr "<<std::endl;
    std::cout << "adjusted lru node use count should be 3? " << pos.use_count() <<std::endl;
    std::cout << "adjusted lru list header prev and next : " << m_candidate_header->prev->BCI->frame_id <<" "<< m_candidate_header->next->BCI->frame_id << std::endl;
    
}
void LRU_Replacer::Remove(int frame_id)
{
   if(0==m_candidate.count(frame_id))
   {
       std::cerr << "frame_id : "<<frame_id<<" Not In LRU"<<std::endl;
   }else{
        auto & target = m_candidate[frame_id];
        target->prev->next=target->next;
        target->next->prev=target->prev;
        m_free.push_back(target->BCI);
        m_candidate.erase(frame_id);
        std::cout << "LRU Remove : " <<frame_id <<std::endl;
   }
}

int bms::FixPage(int p_page_id,int p_protection){
    //if current page inbuffer
    std::cout <<"this page_id "<<p_page_id<<" will be hash to " <<m_hashfunc(p_page_id)<<std::endl;
    auto &bucket = m_allocatedframe[m_hashfunc(p_page_id)];

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
        
        std::cout << "hit for page_id " << p_page_id <<std::endl;
        m_hit++;
        m_replacer.LiftUp((*it)->frame_id);

        //hit
        //...
        
        return (*it)->frame_id;
    }

    //allocate new frame
    //miss -> execute some io
    //...
    std::cout << "miss for page_id : " <<p_page_id <<std::endl;
    m_miss++;
    //if buffer full
    if(0==m_freeframenumber)
    {
        std::cerr << "buffer pool already full" << "\n";
        //evict 
        m_evict++;
        bc_bucket ret = m_replacer.Evict();
        
        auto toremove =m_allocatedframe[m_hashfunc(ret->page_id)];

        std::cout << "after locate bucket to remove evicted BlockControlInfo" <<std::endl;
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
            std::cerr<<"No free frame and Evict Fail!\n"<<std::endl;
            return -1;
        }

        //remove evicted bci from allocated list
        toremove.remove(ret);


        ret->page_id = -1;
        m_freeframenumber++;
        m_frame2page[ret->frame_id]=-1;
    }
     //now buffer is available
     //pick a bcb from freeframe
    auto free_frame = m_freeframe.front();
    m_freeframe.pop_front();

    std::cout << "pick free frame_id "<<free_frame->frame_id<<std::endl;

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
    
    int new_page_id = m_dms->NewPage();
    std::cout << "bms fix new page_id" <<new_page_id << std::endl;
    int new_frame_id = FixPage(new_page_id);
    // no need to SetDirty(new_page_id,true);//new page always dirty
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
    //no need to m_freeframe.push_back(ret);
    m_allocatedframe[m_hashfunc(p_page_id)].remove(ret);
    
    //corresponding frame will be reused but and unset page_id use_bit
    m_freeframenumber++;
    m_frame2page[ret->frame_id]=-1;
    ret->dirty=false;

    m_dms->SetUse(ret->page_id,0);
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
    
    std::cout << "flush back "<<std::endl;
    for(int i=0;i<DEFAULT_BUFFERSIZE;++i)
    {
        if(auto bucket= m_allocatedframe[i];!bucket.empty())
        {
            for(auto  bc: bucket)
            {
                std::cout << i<<"'s bucket member" <<std::endl; 
                if(bc->dirty) m_dms->WritePage(bc->page_id,bc);
                m_freeframe.push_back(bc);
               // bucket.remove(bc);//!!!!!! bug fuck
                std::cout << "bucket remove" << std::endl;
                m_replacer.Remove(bc->frame_id);
                std::cout << "lru remove " <<std::endl;
                bc->dirty=false;
                bc->page_id=-1;
                //bc->pin_count=0;
            }
            bucket.clear();
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


