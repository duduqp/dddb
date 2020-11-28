#include "dms.h"
int dms::OpenFile(std::string p_filename)
{
    curFile.open(p_filename,std::ios::in|std::ios::out);
    if(!curFile) 
    {
        std::cerr<<"File : "<<p_filename<<"Not Exists" << std::endl;
        return -1;
        
    }else{
        //update numpages and curFile-page-use_bit
        
        return 0;
    }

}
int dms::CloseFile()
{
    curFile.close();
}
dms::ptr_bc dms::ReadPage(int page_id,ptr_bc dst){
    //if page_id has not been used or page is empty(no record)
    if(pages[page_id]==0){
        std::cerr<<"Page has not been used or Empty Page!\n";
        return dst;
    }
    //otherwise , read page_id to dst-frame_id
    curFile.seekp(page_id*FRAMESIZE);
    int frame_dst_id = dst->frame_id;
    curFile.read(&bufferpool[frame_dst_id].field[0],FRAMESIZE);
    if(!curFile) std::cerr<<"Read Page : "<<page_id << "Error!\n";
    
    return dst;
}
int dms::WritePage(int page_id,ptr_bc src)
{
    if(pages[page_id]==0) pages[page_id]++;
    //flush to db-file
    int frame_src_id = src->frame_id;
    curFile.seekp(page_id*FRAMESIZE);
    curFile.write(&bufferpool[frame_src_id].field[0],FRAMESIZE);
    if(!curFile.good()) std::cerr<<"Write Page : "<<page_id <<"Error!\n";
    int writen_num = curFile.gcount();
    return writen_num;
}
dms::~dms() {
    CloseFile();
}
void dms::IncNumPages(){
    numPages++;
} 
int dms::GetNumPages()const 
{
    return numPages;
}
void dms::SetUse(int index, int use_bit){
    pages[index]++;
} 
int dms::GetUse(int index) const
{
    return pages[index];
}

