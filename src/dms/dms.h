#pragma once
#include "../include/common.h"

/*
 * a Directory itself is  pages of dir , it manages the pages in a dbfile
 * typically contains checksum free slots and page id page size offset ... 
 * */
class Directory{
public:
    friend std::fstream & operator>>(std::fstream & is,Directory & dir)
    {
        //convenient input
        if(!is||is.is_open()) return is;

        if(dir.page_count>=0) return is;




        char * bitstart = (char *)dir.page_bit;
        is.read(bitstart,FRAMESIZE*2);

        dir.page_count=dir.page_bit[0];
        dir.page_max = dir.page_bit[1];
        dir.page_bitmap=&dir.page_bit[2];
        return is;
    }


    Directory()
    {
        page_count = -1;
    }
    bool IsPageUsed(int page_id)const
    {
        if(page_id>page_max) return false;
        return page_bitmap[page_id/sizeof(int)]&(1U<<(page_id%sizeof(int)));
    }
    int SetPage(int page_id,int use_bit)
    {
        bool flag=IsPageUsed(page_id);
        if(flag&&(use_bit==1)) return 1;
        if(!flag&&(use_bit==0)) return 0;
        if(page_id>=MAXPAGE) return -1;
        int num_32 = page_id/sizeof(int);
        int num_bit = (page_id+1)%sizeof(int);
        if(num_bit== 0 ) num_bit = sizeof(int);
        unsigned int mask = 1U << (num_bit-1);
        if(use_bit) 
        {
            page_bitmap[num_32]|=mask;
            page_count++;
        }else 
        {
            page_bitmap[page_id/sizeof(int)]&=(~mask);
            page_count--;
        }
        return use_bit;
    }

    int FindFreePage() const
    {
        //need metadata page size
        int val = 0;
        //sequential search
        int mask = -1;
        int i;
        for(i=0;i<page_max/sizeof(int);++i)
        {
            if(page_bitmap[i]!=-1) 
            {
                int val = ~page_bitmap[i];
                val=val & (-val);//lowbit
                return i*sizeof(int)+log(val)/log(2);
            }
        }
       int inbyte = 0;
        for(int j=0;j<(page_max+1)%sizeof(int);++j)
        {
            if(((1U<<inbyte)&page_bitmap[i])==0)
            {
                return i*sizeof(int)+inbyte;
            }
        }
        return -1;
    }

    int GetPage()
    {
        if(page_count<=page_max+1)
        {
            int freepage = FindFreePage();
            page_count++;
            return freepage;
        }else{
            if(page_max+1>=MAXPAGE) return -1;
        }

        //allocate next page
        SetPage(page_max+1,1);
        page_max++;page_count++;

        return page_max;
    }


    int GetPageCount() const { return page_count; }
    void Initdir(std::fstream & fs)
    {
        fs >> *this ;
    }
    void Flush(std::fstream & fs)
    {
        fs.seekp(0);
        page_bit[0]=page_count;
        page_bit[1]=page_max;
        char * bitstart=(char*)page_bit;
        fs.write(bitstart,FRAMESIZE*2);
        fs.flush();
    }
private:
    int page_max;
    int page_count;
    int page_bit[FRAMESIZE/2];
    int *page_bitmap;
};


class dms
{
public:
    typedef std::shared_ptr<BlockControlInfo> ptr_bc; 
public:
    dms();
    int OpenFile(std::string p_filename);
    int CloseFile();
    ptr_bc ReadPage(int page_id,ptr_bc dst);
    int WritePage(int page_id,ptr_bc src);
    ~dms() ; 
    void IncNumPages(); 
    int GetNumPages() const ; 
    void SetUse(int index, int use_bit); 
    bool IsUsed(int index) const;
private:  
    std::fstream curFile;
    Directory dbFile_dir;
};  

