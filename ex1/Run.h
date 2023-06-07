#ifndef EX1_RUN_H
#define EX1_RUN_H
#include <vector>
#include <sstream>
#include <unistd.h>
#include "MyFile.h"
#include "VirtualFileSystem.h"

class Run
{
public:
    Run(){};
    ~Run(){};

    void menu();
    bool read(VirtualFileSystem&,std::string, int);
    bool write(VirtualFileSystem&, std::string, int, char);
    void touch(std::string, VirtualFileSystem&);
    bool copy(std::string, std::string, VirtualFileSystem&);
    bool cat(VirtualFileSystem&, std::string);
    bool removeFile(VirtualFileSystem&, std::string);
    bool wc(VirtualFileSystem&, std::string);
    bool ln(VirtualFileSystem&, std::string, std::string);
    void run();

private:
    bool check_char(std::string);
    bool is_param (std::string line);
    std::vector<std::string> file_data(std::string);
};
#endif //EX1_RUN_H