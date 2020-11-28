#pragma once
#include "../include/common.h"



class dms
{
public:
    typedef std::shared_ptr<BlockControlInfo> ptr_bc; 
public:
    dms() {}
    int OpenFile(std::string p_filename);
    int CloseFile();
    ptr_bc ReadPage(int page_id,ptr_bc dst);
    ~dms() {}
    int Seek(int offset, int pos); 
    FILE * GetFile(); 
    void IncNumPages(); 
    int GetNumPages(); 
    void SetUse(int index, int use_bit); 
    int GetUse(int index);
private:  
    std::fstream curFile;
  int numPages;
  int pages[MAXPAGE];
};  

