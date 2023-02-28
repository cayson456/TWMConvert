#pragma once

class FileWriter
{
public:
    FileWriter() {}
    ~FileWriter() {}

    template <typename T>
    inline void Write(const T& val, size_t num_bytes);

    void WriteBlankBytes(size_t num_bytes);

    void WriteToFile(const std::string& filename);
    void PrintBuffer();

private:
    static bool mIsLittleEndian;

    std::vector<std::byte> mBuffer;

};

template<typename T>
inline void FileWriter::Write(const T& val, size_t num_bytes)
{
    const std::byte* byte_start = reinterpret_cast<const std::byte*>(&val);
    mBuffer.insert(mBuffer.end(), byte_start, byte_start + num_bytes);
}

