#include "precompiled.hpp"
#include "FileWriter.hpp"

// Check endianness of processor
union {
    uint32_t i;
    char c[4];
} bint = { 0x01020304 };
bool FileWriter::mIsLittleEndian = !(bint.c[0] == 1);

void FileWriter::WriteBlankBytes(size_t num_bytes)
{
    mBuffer.insert(mBuffer.end(), num_bytes, std::byte{ 0x0 });
}

void FileWriter::WriteToFile(const std::string& filename)
{
    std::ofstream output_filestream(filename, std::ofstream::binary);

    output_filestream.write(reinterpret_cast<char*>(mBuffer.data()), mBuffer.size());
    output_filestream.close();
}

void FileWriter::PrintBuffer()
{
    std::cout << reinterpret_cast<char*>(mBuffer.data()) << std::endl;
}