#ifndef EX1_FILECHARPROXY_H
#define EX1_FILECHARPROXY_H
#include <iostream>

class FileCharProxy
{
public:
    FileCharProxy(std::iostream& stream, int index) : stream(stream), index(index) { }
    operator char() const
    {
        stream.seekg(index);
        char ch = stream.get();
        return ch;
    }
    FileCharProxy& operator=(char ch)
    {
        stream.seekp(index);
        stream.put(ch);
        return *this;
    }
private:
    std::iostream& stream;
    int index;
};
#endif //EX1_FILECHARPROXY_H
