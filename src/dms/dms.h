#pragma once
#include "../include/common.h"
#include <filesystem>
#include <bitset>
#include <utility>
extern const int DIR_PAGE_NUM;
/*
 * a Directory itself is  pages of dir , it manages the pages in a dbfile
 * typically contains checksum free slots and page id page size offset ... 
 * */
class Directory{
public:
    Directory()
    {
        page_count = -1;
    }
    bool IsPageUsed(int page_id)const
    {
        if(page_id>page_max) return false;
        return (page_bitmap[page_id/(sizeof(int)*8)]&(1U<<(page_id%(sizeof(int)*8))))!=0;
    }
    int SetPage(int page_id,int use_bit)
    {
        bool flag=IsPageUsed(page_id);
        if(flag&&(use_bit==1)) return 1;
        if(!flag&&(use_bit==0)) return 0;
        if(page_id>=MAXPAGE) return -1;
        // not used now set it in use 
        
        int num_32 = page_id/(sizeof(int)*8);

        std::cout << "page id : " <<page_id<<std::endl;
        std::cout << "in " << num_32 << " [int]bitmap "<<std::endl;

        int num_bit = (page_id+1)%(sizeof(int)*8);
        if(num_bit==0) num_bit = (sizeof(int)*8);
        unsigned int mask = 1U << (num_bit-1);


        std::cout << "mask : " <<std::bitset<sizeof(int)*8>(mask)<<std::endl;
        std::cout << "~mask : "<<std::bitset<sizeof(int)*8>(~mask)<<std::endl;
        if(use_bit==1) 
        {
            page_bitmap[num_32]=page_bitmap[num_32]|mask;
            page_count++;
        }else 
        {
            page_bitmap[num_32]&=(~mask);
            page_count--;
        }
        ShowPageMap(page_id);
        if(page_id > page_max) page_max=page_id;
        return use_bit;
    }

    int FindFreePage() const
    {
        //need metadata page size
        int val = 0;
        //sequential search
        int mask = -1;// 1111......1111
        int i;
        for(i=0;i<page_max/(sizeof(int)*8);++i)
        {
            if(page_bitmap[i]!=-1) 
            {
                int val = ~page_bitmap[i];
                val=val & (-val);//lowbit
                return i*(sizeof(int)*8)+log(val)/log(2);
            }
        }
       int inbyte = 0;
        for(int j=0;j<(page_max+1)%(sizeof(int)*8);++j)
        {
            if(((1U<<inbyte)&page_bitmap[i])==0)
            {
                return i*(sizeof(int)*8)+inbyte;
            }
            inbyte++;
        }
        return -1;
    }

    int GetPage()
    {
        if(page_count<=page_max)
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
    void Initdir(std::fstream & is)
    {
        //convenient input
        if(!is||!is.is_open()) return ;

        if(page_count>=0) return ;

        std::cout << "begin init dir" <<std::endl;

        is.seekg(0);
        is.read(reinterpret_cast<char *>(&page_bit[0]),FRAMESIZE*2);

        page_count=page_bit[0];
        page_max = page_bit[1];
        page_bitmap=&page_bit[2];

        //debug 

        for(int i=0;i<42;++i)
        {
            std::cout << page_bitmap[i] << " ";
        }
        std::cout << std::endl;
        return ;

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

    void ShowPageMap(int page_id) const
    {    
        int num_32 = page_id / (sizeof(int)*8);
        std::cout<<std::bitset<sizeof(int)*8>(page_bitmap[num_32]);
        std::cout <<std::endl;
    }
private:
    int page_max;
    int page_count;
    int page_bit[FRAMESIZE*2];
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
    int NewPage(){ return dbFile_dir.GetPage(); } 

    int GetNumPages() const ; 
    void SetUse(int index, int use_bit); 
    bool IsUsed(int index) const;

    //for dir test
    Directory GetDirView() const{
        return dbFile_dir;
    }
private:  
    std::fstream curFile;
    Directory dbFile_dir;
};  

