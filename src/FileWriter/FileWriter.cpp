#include "precompiled.hpp"
#include "FileWriter.hpp"
#include "LZ4/lz4.h"

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
    if (mBuffer.size() > LZ4_MAX_INPUT_SIZE) {
        std::cout << "Max Input size Reached" << std::endl;
        return;
    }

    // Compress the bytes
    unsigned int compression_size = static_cast<unsigned int>(LZ4_compressBound(static_cast<int>(mBuffer.size())));
    char* compressed_buffer = new char[compression_size];

    compression_size = LZ4_compress_default(reinterpret_cast<const char*>(mBuffer.data()), compressed_buffer,
        static_cast<int>(mBuffer.size()),
        static_cast<int>(compression_size));

    std::cout << "Compressing data of size: " << mBuffer.size() << " to file: " << filename << " of size: " << compression_size << std::endl;

    std::ofstream output_filestream(filename, std::ofstream::binary);

    uint64_t uncompressed_size = static_cast<uint64_t>(mBuffer.size());
    output_filestream.write(reinterpret_cast<char*>(&uncompressed_size), sizeof(uint64_t));
    output_filestream.write(compressed_buffer, compression_size);
    output_filestream.close();

    delete[] compressed_buffer;
}

void FileWriter::PrintBuffer()
{
    std::cout << reinterpret_cast<char*>(mBuffer.data()) << std::endl;
}