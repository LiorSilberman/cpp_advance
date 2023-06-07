#include "MyFile.h"


bool MyFile::check_file()
{
    std::fstream f;
    f.open(file_name);
    if (!f)
        return false;

    f.close();
    return true;
}


int MyFile::get_size()
{
    std::fstream file;
    file.open(file_name);

    int charCount = 0;
    char c;
    while (file.get(c)) {
        charCount++;
    }
    file.close();
    return charCount;
}


int MyFile::get_ref() const
{
    return ref_count;
}


void MyFile::increment_ref_count()
{
    ref_count++;
}


void MyFile::decrement_ref_count()
{
    if (ref_count > 0) {
        ref_count--;
        if (ref_count == 0) {
            delete this;
        }
    }
}


std::string MyFile::get_name()
{
    return file_name;
}

void MyFile::close_file()
{
    if (fstream.is_open()) {
        fstream.close();
        fstream.clear();
    }
}

 
void MyFile::add_link(MyFile* link)
{
    links.push_back(link);
}


std::vector<MyFile*> MyFile::get_links()
{
    return links;
}
