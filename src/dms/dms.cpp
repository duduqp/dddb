#include "dms.h"
extern const int DIR_PAGE_NUM=2;

extern BufferFrame bufferpool[DEFAULT_BUFFERSIZE];


dms::dms(){
    ;//nothing to do
}




int dms::OpenFile(std::string p_filename)
{
    //is file exists?
    //...
    //
    auto basepath = std::filesystem::current_path();
    std::filesystem::path p{basepath/"data.dbf"};
    std::error_code err{};

    auto isexisted = std::filesystem::exists(p,err);
    if(!isexisted)
    {
        std::cout << "data.dbf is not existed" << std::endl; 
        //then create
        std::ofstream init_stream(p,std::ios::binary|std::ios::app);
        init_stream.seekp(0);

        char zero[2*FRAMESIZE];
        std::fill(zero,zero+2*FRAMESIZE,0);
        init_stream.write(zero,2*FRAMESIZE);
        init_stream.flush();
    }


    //"data.dbf" must exist
    curFile.open(p_filename,std::ios::binary|std::ios::app|std::ios::in|std::ios::out);
    
    if(curFile.fail()) 
    {
        std::cerr<<"File : "<<p_filename<<"Not Exists" << std::endl;
        return -1;    
    }else{
        std::cout << "data.dbf exists" << std::endl;
        //fix size directory format :
        //page_count : int 4B
        //2-page directory bitmap about 60000+ page-use-bit
        
        curFile.seekg(0);
        std::cout << dbFile_dir.GetPageCount()<<std::endl;
        dbFile_dir.Initdir(curFile);
        std::cout << "after initdir page_count : " <<dbFile_dir.GetPageCount()<<std::endl;
        if(dbFile_dir.GetPageCount()==0)//empty
        {
            std::cout << "test dir initial ok?"<<std::endl;

            std::cout << "current free page_id : "<<dbFile_dir.FindFreePage() << std::endl;
            return 0;
            //empty page , initialize it
        }
        
        
        return 0;
    }

}
int dms::CloseFile()
{
    curFile.close();
    //flush back the metadata ...
}
dms::ptr_bc dms::ReadPage(int page_id,ptr_bc dst){
    //if FixNewPage for insert index or ...
    //that must be numpages = page_id + 1;
    if(!dbFile_dir.IsPageUsed(page_id))
    {
        curFile.seekp((page_id+DIR_PAGE_NUM)*FRAMESIZE);
        curFile << "header" << 0;
        curFile.flush();
    }

    dbFile_dir.SetPage(page_id,1);
    //if page_id has not been used or page is empty(no record)

    //otherwise , read page_id to dst-frame_id
    curFile.seekg((page_id+DIR_PAGE_NUM)*FRAMESIZE);
    int frame_dst_id = dst->frame_id;
    curFile.read(&bufferpool[frame_dst_id].field[0],FRAMESIZE);
    if(!curFile.good()) std::cerr<<"Read Page : "<<page_id << "Error!\n";
    
    return dst;
}
int dms::WritePage(int page_id,ptr_bc src)
{
    dbFile_dir.SetPage(page_id,1);
    curFile.seekg((page_id+DIR_PAGE_NUM)*FRAMESIZE);
    char headermsg[6];//"header"
    int page_size=0;
    curFile.read(headermsg,6);
    curFile>>page_size;

    //get page size ...
    std::cout << "Page : " << page_id << "Size : " << page_size << "\n";
    //flush to db-file
    int frame_src_id = src->frame_id;
    curFile.seekp((page_id+DIR_PAGE_NUM)*FRAMESIZE);
    curFile.write(&bufferpool[frame_src_id].field[0],FRAMESIZE);
    if(!curFile.good()) std::cerr<<"Write Page : "<<page_id <<"Error!\n";
    int writen_num = curFile.gcount();
    return writen_num;
}
dms::~dms() {
    CloseFile();
}
int dms::GetNumPages()const 
{
    return dbFile_dir.GetPageCount() ;
}
void dms::SetUse(int index, int use_bit){
    dbFile_dir.SetPage(index,use_bit);
} 
bool dms::IsUsed(int index) const
{
    return dbFile_dir.IsPageUsed(index);
}

