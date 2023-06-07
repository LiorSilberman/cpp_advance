#ifndef EX1_VirtualFileSystem_H
#define EX1_VirtualFileSystem_H
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <set>
#include "MyFile.h"

class VirtualFileSystem
{
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

    
    void mkdir(std::string dirName);
    void chdir(std::string dirName);
    void rmdir(std::string dirName);
    void ls(std::string);
    void lproot() const;
    void pwd() const;
    void addFile(std::vector<std::string>, MyFile *);
    std::vector<std::string> splitSlash(std::string);
    void removeFile(std::string);
    MyFile* get_file(std::string);
    bool isDirExist(std::string);
    bool isFileExist(std::string);
    
private:

    
    struct DirInfo {
        std::string name;
        std::string parent;
        std::map<std::string, DirInfo*> dirs;
        std::map<std::string, MyFile*> files;
    };

    DirInfo* root;
    DirInfo* currentDir;
    DirInfo* allDirs;

    
    std::vector<std::string> pwd(DirInfo* current) const;
    void removeDir(DirInfo*);
    void printDir(DirInfo*) const;
};

#endif