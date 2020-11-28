#include "dms.h"
int dms::OpenFile(std::string p_filename)
{
    curFile.open(p_filename,std::ios::in|std::ios::out);
    if(!curFile) 
    {
        std::cerr<<"File : "<<p_filename<<"Not Exists" << std::endl;
        return -1;
        
    }else{
        return 0;
    }

}
int dms::CloseFile()
{
    curFile.close();
}
dms::ptr_bc dms::ReadPage(int page_id,ptr_bc dst){
    //read page_id to dst-frame_id
    curFile.seekp(page_id*FRAMESIZE);
    int frame_dst_id = dst->frame_id;
    curFile.get(&bufferpool[frame_dst_id].field[0],FRAMESIZE);
    if(!curFile) std::cerr<<"Read Page : "<<page_id << "Error!\n";
    return dst;
}
~dms::dms() {
    CloseFile();
}
int dms::Seek(int offset, int pos){
    curFile.seekp(pos,offset);
} 
FILE * GetFile(); 
void IncNumPages(); 
int GetNumPages(); 
void SetUse(int index, int use_bit); 
int GetUse(int index);

