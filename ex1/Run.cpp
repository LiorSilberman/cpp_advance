#include <iostream>
#include <string>
#include "Run.h"


void Run::menu()
{
    std::cout << "[1]   read FILENAME POSITION" << std::endl;
    std::cout << "[2]   write FILENAME POSITION CHARACTER" << std::endl;
    std::cout << "[3]   touch FILENAME" << std::endl;
    std::cout << "[4]   copy SOURCE_POSITION TARGET_FILENAME" << std::endl;
    std::cout << "[5]   remove FILENAME" << std::endl;
    std::cout << "[6]   move SOURCE_POSITION TARGET_FILENAME" << std::endl;
    std::cout << "[7]   cat FILENAME" << std::endl;
    std::cout << "[8]   wc FILENAME" << std::endl;
    std::cout << "[9]   ln TARGET_FILENAME LINK_NAME" << std::endl;
    std::cout << "[10]  mkdir FOLDERNAME" << std::endl;
    std::cout << "[11]  chdir FOLDERNAME" << std::endl;
    std::cout << "[12]  rmdir FOLDERNAME" << std::endl;
    std::cout << "[13]  ls FOLDERNAME" << std::endl;
    std::cout << "[14]  lproot" << std::endl;
    std::cout << "[15]  pwd" << std::endl;
    std::cout << "[16]   exit" << std::endl;
}

bool Run::read(VirtualFileSystem& vfs, std::string file_name, int index)
{
    std::vector<std::string> path = vfs.splitSlash(file_name);
    std::string fileName = path.back();
    // check dir path
    if (path.size() != 1)
    {
        path.pop_back();
        for (std::string dir : path)
            if (!vfs.isDirExist(dir))
                return false;
    }

    MyFile myFile(fileName);

    // check if file exist
    if (!myFile.check_file())
        return false;

    // index out if bounds
    if (index < 0 || index > myFile.get_size())
        return false;

    // read from file by index
    if ((myFile.get_size() == 0 && index == 0) || myFile.get_size() == index)
        return true;

    std::cout << myFile[index] << std::endl;
    return true;
}

bool Run::write(VirtualFileSystem& vfs, std::string file_name, int index, const char character)
{
    std::vector<std::string> path = vfs.splitSlash(file_name);
    std::string fileName = path.back();
    // check dir path
    if (path.size() != 1)
    {
        path.pop_back();
        for (std::string dir : path)
            if (!vfs.isDirExist(dir))
                return false;
    }

    MyFile myFile(fileName);

    // check if file exist
    if (!myFile.check_file())
        return false;

    // index out of bounds
    if (index < 0 || index > myFile.get_size())
        return false;

    myFile[index] = character;
    
    MyFile* tempFile = vfs.get_file(fileName);

    for (auto file : tempFile->get_links())
    {
        MyFile temp(file->get_name());
        temp[index] = character;
        // copy(myFile.get_name(), file->get_name(), vfs);
    }

    
    return true;
}

void Run::touch(std::string file_name, VirtualFileSystem& vfs)
{
    std::vector<std::string> vec_res = vfs.splitSlash(file_name);
    std::string fileName = vec_res.back();
    

    MyFile myFile(fileName);
    // check if file already exist
    if (!myFile.check_file())
    {
        // create new file
        std::ofstream f(fileName);
        f.close();
        MyFile* temp = new MyFile(fileName);

        // add the file to virtual folder
        vfs.addFile(vec_res, temp);
        return;
    }

    // file exist but not in VFS
    if (!vfs.isFileExist(fileName))
    {
        MyFile* temp = new MyFile(fileName);
        vfs.addFile(vec_res, temp);
    }
    
    // file exist, touch without change data
    char temp = myFile[0];
    myFile[0] = temp;
}

bool Run::copy(std::string source, std::string target, VirtualFileSystem& vfs)
{
    std::vector<std::string> pathSource = vfs.splitSlash(source);
    std::vector<std::string> pathTarget = vfs.splitSlash(target);
    std::string sourceName = pathSource.back();
    std::string targetName = pathTarget.back();

    MyFile source_file(sourceName);
    MyFile target_file(targetName);

    // check if source file exist
    if (!source_file.check_file())
        return false;

    // if target file not exist, create him
    if (!vfs.isFileExist(targetName))
        touch(target, vfs);


    // copy to exist file, clear the new file
    std::ofstream ofs;
    ofs.open(targetName, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    MyFile new_target = target_file;
    int size = source_file.get_size();
    // copy source to target
    for (int i = 0; i < size; i++)
    {
        char ch = source_file[i];
        new_target[i] = ch;
    }

    return true;
}

bool Run::cat(VirtualFileSystem& vfs, std::string file_name)
{
    std::vector<std::string> path = vfs.splitSlash(file_name);
    std::string fileName = path.back();
    // check dir path
    if (path.size() != 1)
    {
        path.pop_back();
        for (std::string dir : path)
            if (!vfs.isDirExist(dir))
                return false;
    }

    MyFile myFile(fileName);
    // check if file exist
    if (!myFile.check_file())
        return false;

    if (myFile.get_size() == 0)
        return true;

    int size = myFile.get_size();
    for (int i = 0; i < size; i++)
    {
        std::cout << myFile[i];
    }
    std::cout << std::endl;
    myFile.close_file();
    return true;
}

bool Run::removeFile(VirtualFileSystem& vfs ,std::string file_name)
{
    std::vector<std::string> path = vfs.splitSlash(file_name);
    std::string fileName = path.back();
    // check dir path
    if (path.size() != 1)
    {
        path.pop_back();
        for (std::string dir : path)
            if (!vfs.isDirExist(dir))
                return false;
    }
    // get the file from virtual system files
    MyFile* myFile = vfs.get_file(fileName);

    // check if file exists
    if (myFile == nullptr)
        return false;

    // decrement the reference count before removing the file
    for (MyFile* link : myFile->get_links()) {
        link->decrement_ref_count();
    }

    // remove the file from system
    if (remove(fileName.c_str()) != 0)
        return false;

    // remove the file from virtual system files
    vfs.removeFile(fileName);
    
    return true;
}

bool Run::wc(VirtualFileSystem& vfs, std::string file_name)
{
    std::vector<std::string> path = vfs.splitSlash(file_name);
    std::string fileName = path.back();
    // check dir path
    if (path.size() != 1)
    {
        path.pop_back();
        for (std::string dir : path)
            if (!vfs.isDirExist(dir))
                return false;
    }

    MyFile myFile(fileName);
    // check if file exist
    if (!myFile.check_file())
        return false;

    int num_words = 0;
    int num_chars = myFile.get_size();;
    int num_lines = 0;

    bool insideWord = false;

    // count number of words
    for (int i = 0; i < num_chars; i++)
    {
        if (myFile[i] == ' ' || myFile[i] == '\t' || myFile[i] == '\n' || myFile[i] == '\r') {
            if (insideWord) {
                num_words++;
                insideWord = false;
            }

            // count number of lines
            if (myFile[i] == '\n')
                num_lines++;
        }
        else {
            insideWord = true;
        }
    }  
    if (insideWord)
        num_words++;
    

    // display lines/words/chars
    std::cout << num_lines << "/" << num_words << "/" << num_chars << std::endl;
    return true;
}

bool Run::ln(VirtualFileSystem& vfs, std::string file_name, std::string link_name)
{
    std::vector<std::string> path1 = vfs.splitSlash(file_name);
    std::vector<std::string> path2 = vfs.splitSlash(link_name);
    std::string fileName = path1.back();
    std::string linkName = path2.back();
    
    // check original file - dir path
    if (path1.size() != 1)
    {
        path1.pop_back();
        for (std::string dir : path1)
            if (!vfs.isDirExist(dir))
                return false;
    }   
    
    MyFile* myFile = vfs.get_file(fileName);
    
    if (myFile == nullptr)
        return false;
    

    // check if link file already exists in virtual system
    if (vfs.isFileExist(linkName)) {
        return false;
    }
    
    myFile->increment_ref_count();

    // Add linked file to VFS with ref count of 1 delete him if the path is'nt right
    try {
        touch(link_name, vfs);
        copy(file_name, link_name, vfs);
        vfs.get_file(linkName)->add_link(myFile);
        MyFile link(linkName);
        vfs.get_file(fileName)->add_link(vfs.get_file(link.get_name()));

    } catch (std::string ex){
        myFile->decrement_ref_count();
        throw ex;
    }
    
    
    return true;
}

void Run::run()
{
    VirtualFileSystem vfs;
    while (true)
    {
//        menu();
        std::string line;
        try {
            getline(std::cin, line);

            if (line == "exit")
                break;

            // split the line by spaces
            std::stringstream ss(line);
            std::vector<std::string> vec_res;
            std::string tok;
            while (getline(ss, tok, ' '))
                vec_res.push_back(tok);


            // call read function
            if (vec_res[0] == "read")
            {
                if (vec_res.size() != 3 || !is_param(vec_res[2]))
                    throw "error: USAGE " + vec_res[0] + " FILENAME POSITION.\n";

                if (!read(vfs, vec_res[1], std::stoi(vec_res[2])))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";

                continue;

            }

            // call write function
            if (vec_res[0] == "write")
            {
                // check input
                if (vec_res.size() != 4 || !check_char(vec_res[3]))
                    throw "error: USAGE "+ vec_res[0] + " FILENAME POSITION CHARACTER.\n";



                // character to write inside the file
                char c = vec_res[3][0];
                if (vec_res[3] == "")
                    c = ' ';

                if (!write(vfs, vec_res[1], std::stoi(vec_res[2]), c))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";

                continue;
            }

            if (vec_res[0] == "touch")
            {
                // check input
                if (vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FILENAME.\n";

                touch(vec_res[1], vfs);
                
                continue;
            }

            if (vec_res[0] == "copy")
            {
                // check input
                if (vec_res.size() != 3)
                    throw "error: USAGE "+ vec_res[0] + " SOURCE_FILENAME TARGET_FILENAME.\n";

                if (!copy(vec_res[1], vec_res[2], vfs))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";
                continue;
            }

            if (vec_res[0] == "remove")
            {
                // check input
                if (vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FILENAME.\n";

                

                if (!removeFile(vfs, vec_res[1]))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";

                continue;
            }

            if (vec_res[0] == "move")
            {
                // check input
                if (vec_res.size() != 3)
                    throw "error: USAGE "+ vec_res[0] + " SOURCE_FILENAME TARGET_FILENAME.\n";

                // check if source file is target file
                if (vec_res[1] == vec_res[2]) {
                    // touch(vec_res[1]);
                    continue;
                }
                // try to copy the file
                if (!copy(vec_res[1], vec_res[2], vfs))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";

                // remove the source file
                if (!removeFile(vfs, vec_res[1]))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";

                continue;
            }

            if (vec_res[0] == "cat")
            {
                
                // check input
                if (vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FILENAME.\n";

                std::vector<std::string> fileName = vfs.splitSlash(vec_res[1]);

                if (!cat(vfs, vec_res[1]))
                    throw "error: " + fileName.back() + " does not exist or cannot be processed.\n";

                continue;
            }

            if (vec_res[0] == "wc")
            {
                // check input
                if (vec_res.size() != 2 && vec_res.size() != 3)
                    throw "error: USAGE "+ vec_res[0] + " FILENAME [n].\n";

                
                wc(vfs, vec_res[1]);

                continue;
            }

            if (vec_res[0] == "ln")
            {
                // check input
                if (vec_res.size() != 2 && vec_res.size() != 3)
                    throw "error: USAGE "+ vec_res[0] + " TARGET_FILENAME LINK_NAME.\n";


                // call ln
                if (!ln(vfs, vec_res[1], vec_res[2]))
                    throw "error: " + vec_res[1] + " does not exist or cannot be processed.\n";
                
                continue;
            }

            if (vec_res[0] == "pwd")
            {
                vfs.pwd();
                continue;
            }

            if (vec_res[0] == "mkdir")
            {
                // check input
                if (vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FOLDERNAME.\n";
                
                vfs.mkdir(vec_res[1]);
                continue;
            }

            if (vec_res[0] == "chdir")
            {
                // check input
                if (vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FOLDERNAME.\n";

                vfs.chdir(vec_res[1]);
                continue;
            }

            if (vec_res[0] == "rmdir")
            {
                // check input
                if (vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FOLDERNAME.\n";
                
                vfs.rmdir(vec_res[1]);
                continue;
            }

            if (vec_res[0] == "ls")
            {
                std::string currentDir = "";
                std::vector<std::string> dirName;
                // check input
                if (vec_res.size() != 1 && vec_res.size() != 2)
                    throw "error: USAGE "+ vec_res[0] + " FOLDERNAME.\n";

                if (vec_res.size() == 2)
                {
                    dirName = vfs.splitSlash(vec_res[1]);
                    vfs.ls(dirName.back());
                    continue;
                }
                else 
                {
                    vfs.ls(currentDir);
                    continue;
                }
                    
            }

            if (vec_res[0] == "lproot")
            {
                if (vec_res.size() != 1)
                    throw "error: USAGE "+ vec_res[0] + "\n";

                vfs.lproot();
            }
                

            // else , no command found
            else
            {
                std::string s = "error: command is not supported.\n";
                throw s;
            }
        }
        catch (std::string s)
        {
            std::cout << s << std::endl;
        }
    }
}

bool Run::check_char(std::string character)
{
    if (character == "")
        return true;

    if (character.size() != 1)
        return false;
    return true;
}

bool Run::is_param(std::string line)
{
    for (char c : line)
        if (!isdigit(c))
            return false;
    return true;
}

// std::vector<std::string> Run::file_data(std::string file_name)
// {
//     std::ifstream f_read(file_name);
//     std::vector<std::string> data;
//     std::string line;

//     // get size of lines in file
//     while (getline(f_read, line))
//         data.push_back(line);

//     f_read.close();
//     return data;
// }

