#include "VirtualFileSystem.h"


VirtualFileSystem::VirtualFileSystem()
{
    root = new DirInfo();
    allDirs = new DirInfo();
    allDirs->name = "V";
    allDirs->parent = "V";
    root->name = "V";
    root->parent = "V";
    currentDir = root;
    allDirs->dirs[root->name] = root;
}

VirtualFileSystem::~VirtualFileSystem()
{
    delete root;
}

void VirtualFileSystem::mkdir(std::string dirName)
{
    std::vector<std::string> path = splitSlash(dirName);
    dirName = path.back();
    DirInfo* current;
    // check dir path and get target dir
    if (path.size() != 1)
    {
        path.pop_back();
        for (std::string dir : path)
            if (!isDirExist(dir))
                throw dir + "/ Directory not found";
        
        current = allDirs->dirs[path.back()];
    } 

    if (path.size() == 1)
        current = currentDir;

    // create dir and insert to the system
    DirInfo* newDir = new DirInfo();
    newDir->name = dirName;
    newDir->parent = current->name;
    current->dirs[dirName] = newDir;
    allDirs->dirs[dirName] = newDir;
    
}

void VirtualFileSystem::chdir(std::string dirName) 
{
    std::vector<std::string> line = splitSlash(dirName);
    
    for (auto it = line.begin(); it != line.end() - 1; ++it) {
        if (!isDirExist(*it))
            throw *it + "/ Directory not found";
    }

    if (isDirExist(line.back()))
        currentDir = allDirs->dirs[line.back()];

    else {throw line.back() + "/ Directory not found";}

}

void VirtualFileSystem::rmdir(std::string dirName)
{
    std::vector<std::string> line = splitSlash(dirName);

    for (auto it = line.begin(); it != line.end(); ++it) {
        if (!isDirExist(*it))
            throw *it + "/ Directory not found";
    }

    DirInfo* dir = allDirs->dirs[line.back()];
    if (!dir) {
        throw line.back() + "/ Directory not found";
    }

    if (line.back().compare("V") == 0)
    {
        std::string s = "Can't remove main root directory";
        throw s;
    }
         

    if (!dir->dirs.empty() || !dir->files.empty()) {
        DirInfo* parent = allDirs->dirs[dir->parent];
        chdir(parent->name);
        // Directory is not empty, recursively delete all files and subdirectories inside it
        removeDir(dir);
    }

    else {
        // Directory is empty, just remove it from the parent's directory list
        DirInfo* parent = allDirs->dirs[dir->parent];
        parent->dirs.erase(dir->name);
        delete dir;
    }
}

void VirtualFileSystem::ls(std::string dirName)
{
    DirInfo* dir = currentDir;
        
    if (!isDirExist(dirName) && dirName != "")
    {
        std::string err = "Directory not found";
        throw err;
    }
        
    if (dirName != "")
    {
        dir = allDirs->dirs[dirName];
    }
    int i = 0;
    for (auto it = dir->files.begin(); it != dir->files.end(); ++it) {
        std::cout << it->first << "\t";
        i++;
        if (i % 6 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

void VirtualFileSystem::lproot() const
{
    DirInfo* dir = root;
    // root dir
    std::cout << dir->name << "/" << std::endl;

    for (auto file = dir->files.begin(); file != dir->files.end(); ++file) 
    {
        std::cout << file->first << " " << file->second->get_ref() <<std::endl;
    }

    for (auto it = dir->dirs.begin(); it != dir->dirs.end(); ++it) {
        printDir(it->second);
    }
}

void VirtualFileSystem::pwd() const
{
    DirInfo* dir = currentDir;
    std::vector<std::string> path;

    while (dir != root) 
    {
        path.push_back(dir->name);
        dir = allDirs->dirs[dir->parent];
    }
    path.push_back(root->name);
    for (int i = path.size() - 1; i >= 0; --i) {
        std::cout << path[i] << "/";
    }
    std::cout << std::endl;
}

std::vector<std::string> VirtualFileSystem::pwd(DirInfo* current) const
{
    DirInfo* dir = current;
    std::vector<std::string> path;

    while (dir != root) 
    {
        path.push_back(dir->name);
        dir = allDirs->dirs[dir->parent];
    }
    path.push_back(root->name);

    return path;
}

bool VirtualFileSystem::isDirExist(std::string dirName)
{
    if (allDirs->dirs.count(dirName) == 0)
        return false;

    return true;
}


bool VirtualFileSystem::isFileExist(std::string fileName)
{
    if (allDirs->files.count(fileName) == 0)
        return false;

    return true;
}

void VirtualFileSystem::addFile(std::vector<std::string> path, MyFile* file)
{
    if (path.size() == 1)
    {
        if (!isDirExist(currentDir->name))
        {
            remove(file->get_name().c_str());
            throw "Directory not found";
        }
        if (!isFileExist(file->get_name()))
        {   
            currentDir->files[file->get_name()] = file;
            allDirs->files[file->get_name()] = file;
            return;
        }
        
    }

    path.pop_back();
    for (auto it = path.begin(); it != path.end(); ++it) {
        if (!isDirExist(*it))
        {
            remove(file->get_name().c_str());
            throw *it + "/ Directory not found";
        }
            
    }

    DirInfo* dir = allDirs->dirs[path.back()];
    if (!dir) {
        throw path.back() + "/ Directory not found";
    }
    if (!isFileExist(file->get_name()))
    {   
        dir->files[file->get_name()] = file;
        allDirs->files[file->get_name()] = file;
        return;
    }
}

MyFile* VirtualFileSystem::get_file(std::string file_name)
{
    if (isFileExist(file_name))
        return allDirs->files[file_name];

    MyFile file(file_name);
    if (file.check_file())
    {
        MyFile* exitstFile = new MyFile(file);
        return exitstFile;
    }
    return nullptr;
}

std::vector<std::string> VirtualFileSystem::splitSlash(std::string line)
{
    // split the line by back slash
    std::stringstream ss(line);
    std::vector<std::string> vec_res;
    std::string tok;
    while (getline(ss, tok, '/'))
        vec_res.push_back(tok);
    
    return vec_res;
}


void VirtualFileSystem::removeDir(DirInfo* dir)
{
    // Recursively delete all files and subdirectories inside this directory
    for (auto it = dir->files.begin(); it != dir->files.end(); ++it) {
        remove((it->first).c_str());
        std::cout << it->first << std::endl;
    }
    dir->files.clear();
    
    for (auto it = dir->dirs.begin(); it != dir->dirs.end(); ++it) {
        if (it->second != nullptr) 
            removeDir(it->second);
    }

    dir->dirs.clear();
    root->dirs.erase(dir->name);
    allDirs->dirs.erase(dir->name);

    // Delete this directory
    delete dir;
}

void VirtualFileSystem::removeFile(std::string fileName)
{
    DirInfo* dir = currentDir;

    for (auto it = dir->dirs.begin(); it != dir->dirs.end(); ++it) {
        delete it->second->files[fileName];
        it->second->files.erase(fileName);
    }

    // delete root->files[fileName];
    // delete allDirs->files[fileName];
    root->files.erase(fileName);
    allDirs->files.erase(fileName);
}


void VirtualFileSystem::printDir(DirInfo* dir) const
{
    std::vector<std::string> path;

    // print directory path and files
    if (dir->dirs.size() < 2)
    {
        path = pwd(dir);
        for (int i = path.size() - 1; i >= 0; --i) {
            std::cout << path[i] << "/";
        }
        std::cout << std::endl;
        // print directory files
        DirInfo* temp = dir;
        for (auto file = temp->files.begin(); file != temp->files.end(); ++file) 
        {
            std::cout << file->first << "\t" << file->second->get_ref() <<std::endl;
        }
    }

    for (auto it = dir->dirs.begin(); it != dir->dirs.end(); ++it) {
        path = pwd(it->second);
        for (int i = path.size() - 1; i >= 0; --i) {
            std::cout << path[i] << "/";
        }
        std::cout << std::endl;

        // print directory files
        DirInfo* temp = it->second;
        for (auto file = temp->files.begin(); file != temp->files.end(); ++file) 
        {
            std::cout << file->first << "\t" << file->second->get_ref() <<std::endl;
        }

        // Recursively call each directory next path if has 
        if (!it->second->dirs.empty())
            printDir(it->second->dirs.begin()->second);
    }

}