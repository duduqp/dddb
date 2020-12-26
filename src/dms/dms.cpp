#include "dms.h"
#include <cstring>
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
        std::ofstream init_stream(p,std::ios::binary);
        init_stream.seekp(0,std::ios::beg);

        char zero[2*FRAMESIZE];
        std::fill(zero,zero+2*FRAMESIZE,0);
        init_stream.write(zero,2*FRAMESIZE);
        init_stream.flush();
    }


    //"data.dbf" must exist
    curFile.open(p_filename,std::ios::binary|std::ios::in|std::ios::out);

    //! seek to 0
    curFile.seekg(0,std::ios::beg);
    curFile.seekp(0,std::ios::beg);
    assert(curFile.tellg()==0);
    assert(curFile.tellp()==0);
    if(curFile.fail()) 
    {
        std::cerr<<"File : "<<p_filename<<"Not Exists" << std::endl;
        return -1;    
    }else{
        std::cout << "data.dbf exists" << std::endl;
        //fix size directory format :
        //page_count : int 4B
        //2-page directory bitmap about 60000+ page-use-bit
        
//        std::cout << dbFile_dir.GetPageCount()<<std::endl;
        dbFile_dir.Initdir(curFile);
//        std::cout << "after initdir page_count : " <<dbFile_dir.GetPageCount()<<std::endl;
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
void dms::CloseFile()
{
    dbFile_dir.Flush(curFile);
     curFile.close();
    //flush back the metadata ...
    //to do
}
dms::ptr_bc dms::ReadPage(int page_id,ptr_bc dst){
    //if FixNewPage for insert index or ...
    //that must be numpages = page_id + 1;
    if(dbFile_dir.GetPageMax()==page_id)//a freshly new allocate page
    {
        curFile.seekp((page_id+DIR_PAGE_NUM)*FRAMESIZE,std::ios::beg);
        char headermsg[FRAMESIZE]="this is a header";
        //assert(16==sizeof(headermsg));
        curFile.write(headermsg,strlen(headermsg)+1);
        std::cout << "after init header , io state "<<curFile.fail()<<std::endl;
        curFile.flush();
    }

    dbFile_dir.SetPage(page_id,1);
    //if page_id has not been used or page is empty(no record)

    //otherwise , read page_id to dst-frame_id
    curFile.seekg((page_id+DIR_PAGE_NUM)*FRAMESIZE,std::ios::beg);
    int frame_dst_id = dst->frame_id;
    std::cout << "Then will read page_id : "<<page_id<<" to frame_id : "<<frame_dst_id<<std::endl;

    curFile.read((char *)(&bufferpool[frame_dst_id].field[0]),FRAMESIZE);
    if(curFile.fail()&&!curFile.eof())
    {
        std::cerr<<"Read Page : "<<page_id << " causes Error!\n";
    }
    curFile.clear();
    return dst;
}
int dms::WritePage(int page_id,ptr_bc src)
{
    dbFile_dir.SetPage(page_id,1);
    curFile.seekg((page_id+DIR_PAGE_NUM)*FRAMESIZE,std::ios::beg);
    char headermsg[17];//"this is a header\0"
    curFile.read(headermsg,17);

    std::cout << curFile.fail() <<std::endl;
    //get page size ...
    std::cout << "Page : " << page_id <<" \n";
    //flush to db-file
    int frame_src_id = src->frame_id;
    curFile.clear();
    curFile.seekp((page_id+DIR_PAGE_NUM)*FRAMESIZE,std::ios::beg);
    curFile.write((char *)(&bufferpool[frame_src_id].field[0]),FRAMESIZE);
    std::cout <<"display frame_id : "<<frame_src_id<<"\n"<< bufferpool[frame_src_id].field << std::endl;
    if(curFile.fail()) std::cerr<<"Write Page : "<<page_id <<" Error!\n";
    curFile.clear();
    int end = curFile.tellp();
    int writen_num= end-(page_id+DIR_PAGE_NUM)*FRAMESIZE;
    std::cout << " Success writing " << writen_num << std::endl;
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

