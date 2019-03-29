#include <stdio.h>
#include <string>
#include "pdfile.h"
#include "mmap.h"
#include "util.h"
#include "datafile_manager.h"

using namespace std;

#include <iostream>

static void writeData(FILE *fp, const char *buf, size_t size)
{
    size_t toWrite = size;
    size_t written = 0;

    while (toWrite)
    {
        size_t ret = fwrite(buf + written, 1, toWrite, fp);
        toWrite -= ret;
        written += ret;
    }
}

#include <string>
#include <bitset>

string two_char_to_binary(string s)   // s is a string of 2 characters of the input string
{
    bitset<8> a (s[0]);    // bitset constructors only take integers or string that consists of 1s and 0s e.g. "00110011"
    bitset<8> b (s[1]);    // The number 8 represents the bit depth

    bitset<16> ans (a.to_string() + b.to_string()); // We take advantage of the bitset constructor that takes a string of 1s and 0s and the concatenation operator of the C++ string class

    return ans.to_string();
}


// dump all record from specified collection
// output file is collname.bson, it can be used by mongoresotre
// Usage: ./dump dbpath dbname collname
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: ./dump dbpath dbname collname\n");
        return -1;
    }

    std::string dbpath(argv[1]);
    std::string dbname(argv[2]);
    std::string collname = dbname + "." + argv[3];

    // 加载namespace文件
    std::string nsFileName = dbpath + "/" + dbname + ".ns";
    NamespaceDetails details;
    if (!findNamespaceDetails(nsFileName, collname, details))
        return -1;

    // 创建要dump出的bson文件
    std::string bsonFilename = std::string(argv[3]) + ".bson";
    FILE *fp = fopen(bsonFilename.c_str(), "wb");
    if (fp == NULL)
    {
        printf("open file %s fail\n", bsonFilename.c_str());
        return -1;
    }

    // 代码等价于java的
    // DatafileManager datafileManager = new DatafileManager(dbpath, dbname);
    DatafileManager datafileManager(dbpath, dbname);

    int x =0;

    DiskLoc loc = details._firstExtent;
    while (!loc.isNull())
    {
        Extent *extent = (Extent *)datafileManager.getView(loc._a, loc.ofs);
        if (extent == NULL)
        {
            printf("datafile error!\n");
            return -1;
        }

        x++;

        // string str0(extent->_extentData);
        printf("====%d\n", x);
        // cout << two_char_to_binary(str0) << endl;

        DiskLoc recordLoc = extent->firstRecord;
        while (!recordLoc.isNull() && (recordLoc.ofs != DiskLoc::NullOfs))
        {
            Record *record = (Record *)datafileManager.getView(recordLoc._a, recordLoc.ofs);
            if (record == NULL)
            {
                printf("datafile error!\n");
                return -1;
            }

            // the size of Bsonobj may not equal to the len of Record
            int len = *(reinterpret_cast<const int *>(record->_data));
            writeData(fp, record->_data, len);

            recordLoc.ofs = record->_nextOfs;
        }

        loc = extent->xnext;
    }

    fclose(fp);
    return 0;
}
