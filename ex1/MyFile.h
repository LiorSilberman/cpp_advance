#ifndef EX1_MYFILE_H
#define EX1_MYFILE_H
#include "FileCharProxy.h"
#include <fstream>
#include <iostream>
#include <vector>

class MyFile
{
public:

    MyFile(std::string file_name) :file_name(file_name), fstream(file_name), ref_count(1) {}                // c'tor
                                      

    MyFile(const MyFile& file) :file_name(file.file_name), fstream(file.file_name), ref_count(file.ref_count){}         // copy c'tor        
    
    MyFile(std::string file_name, int ref_count) :file_name(file_name), fstream(file_name), ref_count(ref_count){} // new constructor
    
    ~MyFile() 
    {   
        close_file();                                                                                               // Destructor
    }

    FileCharProxy operator[](int index) {
        return FileCharProxy{fstream, index};
    }

    void close_file();
    bool check_file();
    int get_size();
    int get_ref() const;
    void increment_ref_count();
    void decrement_ref_count();
    std::string get_name();
    void add_link(MyFile*);
    void remove_link(MyFile*);
    std::vector<MyFile*> get_links();
    
private:
    std::vector<MyFile*> links;
    std::string file_name;
    std::fstream fstream;
    int ref_count;
};

#endif //EX1_MYFILE_H
