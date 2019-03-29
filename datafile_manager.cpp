#include <stdio.h>
#include "datafile_manager.h"
#include "mmap.h"

static const int MaxFiles = DiskLoc::MaxFiles;

DatafileManager::DatafileManager(const std::string& dbpath, const std::string& dbname)
    : dbpath_(dbpath),
      dbname_(dbname),
      mmfiles_(MaxFiles)
{

}

void* DatafileManager::getView(int n, int offset)
{
    // 一个db中允许生成的最大文件数量
    if (n >= MaxFiles)
        return NULL;

    if (mmfiles_[n])
        return (char*)mmfiles_[n]->map() + offset;

    char buf[100];
    sprintf(buf, "%d", n);

    // data file的文件名
    std::string filename = dbpath_ + "/" + dbname_ + "." + buf;
    printf("mmap.filename=%s\n", filename.c_str());

    // 从mmp文件中读取data file的视图
    mmfiles_[n] = new MemoryMappedFile(filename.c_str());
    return (char*)mmfiles_[n]->map() + offset;
}



