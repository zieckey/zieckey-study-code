#ifndef _BUFFER_WRITER_H_
#define _BUFFER_WRITER_H_

#include <stdio.h>

#include "qoslib/include/QOSLibAllExp.h"
#include "qoslib/include/QDataStream.h"

#include "file_writer.h"

class BufferWriter : public FileWriter
{
public:
    BufferWriter(FILE* fp);

    virtual ~BufferWriter() 
    { 
        fp_ = NULL; 
    }

    virtual bool Init() { return true; }

    virtual bool Write(const void* data, size_t len);

    virtual bool Flush();


protected:
    size_t dump_buffer_max_;

    osl::MemoryDataStream output_buf_;

private:
    std::string GetTempOutputFilePath(int64_t file_id);
};

#endif 

