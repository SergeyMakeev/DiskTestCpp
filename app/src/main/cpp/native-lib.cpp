#include <jni.h>
#include <string>
#include <string>
#include <cstdio>
#include <vector>
#include <stdexcept>
#include <random>

template<typename... Args>
std::string format(const std::string& format, Args... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::vector<char> buf(size);
    snprintf(buf.data(), size, format.c_str(), args...);
    return std::string(buf.data(), buf.size() - 1); // We don't want the '\0' inside
}


// The target size of the file in bytes
const long long targetSize = 128LL * 1024 * 1024;
const long numRandomOffsets = 400000;

long* getRandomOffsets()
{
    static long* offsets = nullptr;
    if (offsets == nullptr)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, targetSize - 1024*1024);
        offsets = (long*)malloc(numRandomOffsets * sizeof(long));
        for (int i = 0; i < numRandomOffsets; ++i)
        {
            offsets[i] = dis(gen);
        }
    }
    return offsets;
}


char* getMemoryBuffer()
{
    static char* buffer = nullptr;
    if (!buffer)
    {
        buffer = (char*)malloc(targetSize);
        srand(33);
        for (size_t i = 0; i < targetSize; ++i)
        {
            buffer[i] = static_cast<char>(rand() % 256);
        }
    }
    return buffer;
}

bool create_temp_file(const char* fileName)
{
    FILE* file = fopen(fileName, "wb");
    if (!file)
    {
        printf("Failed to create file");
        return false;
    }
    char* buffer = getMemoryBuffer();
    fwrite(buffer, 1, targetSize, file);
    fclose(file);
    return true;
}

bool read_file_seq(const char* fileName)
{
    FILE* file = fopen(fileName, "rb");
    if (!file)
    {
        printf("Failed to open file");
        return false;
    }
    char* buffer = getMemoryBuffer();
    fread(buffer, 1, targetSize, file);
    fclose(file);
    return true;
}

size_t read_file_rnd(const char* fileName, size_t chunkSizeInBytes)
{
    FILE* file = fopen(fileName, "rb");
    if (!file)
    {
        printf("Failed to open file");
        return 0;
    }

    long* offsets = getRandomOffsets();
    char* buffer = getMemoryBuffer();

    size_t numChunks = size_t(targetSize / (long long)chunkSizeInBytes);
    for(size_t i = 0; i < numChunks; i++)
    {
        fseek(file, offsets[i], SEEK_SET);
        fread(buffer, 1, chunkSizeInBytes, file);
    }

    fclose(file);
    return numChunks * chunkSizeInBytes;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_disktestcpp_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */,
        jstring dirPath)
{
    // to init the memory buffer
    getMemoryBuffer();
    getRandomOffsets();

    std::string path = env->GetStringUTFChars(dirPath, nullptr);
    std::string fileName = path + "/temp.big";

    std::string txt;
    txt = fileName;

    // seq write
    {
        clock_t start = clock();
        bool res = create_temp_file(fileName.c_str());
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;

        if (res) {
            txt += format("\nSuccessfully created in %3.2f sec (%3.2f Mb/sec)", elapsed, 128.0 / elapsed);
        } else {
            txt += "\nCan't create temp file";
        }
    }

    // seq read
    {
        clock_t start = clock();
        bool res = read_file_seq(fileName.c_str());
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (res) {
            txt += format("\nSuccessfully read_seq in %3.2f sec (%3.2f Mb/sec)", elapsed, 128.0 / elapsed);
        } else {
            txt += "\nCan't open temp file";
        }
    }

    // rnd read
    {
        size_t blockSizeInKb = 1;
        clock_t start = clock();
        size_t bytesRead = read_file_rnd(fileName.c_str(), blockSizeInKb*1024);
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (bytesRead > 0) {
            txt += format("\nSuccessfully read_rnd in %3.2f sec (%3.2f Mb/sec) (%d Kb/block)", elapsed, ((double)bytesRead / 1024.0 / 1024.0) / elapsed, int(blockSizeInKb));
        } else {
            txt += "\nCan't open temp file";
        }
    }

    // rnd read
    {
        size_t blockSizeInKb = 4;
        clock_t start = clock();
        size_t bytesRead = read_file_rnd(fileName.c_str(), blockSizeInKb*1024);
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (bytesRead > 0) {
            txt += format("\nSuccessfully read_rnd in %3.2f sec (%3.2f Mb/sec) (%d Kb/block)", elapsed, ((double)bytesRead / 1024.0 / 1024.0) / elapsed, int(blockSizeInKb));
        } else {
            txt += "\nCan't open temp file";
        }
    }

    // rnd read
    {
        size_t blockSizeInKb = 8;
        clock_t start = clock();
        size_t bytesRead = read_file_rnd(fileName.c_str(), blockSizeInKb*1024);
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (bytesRead > 0) {
            txt += format("\nSuccessfully read_rnd in %3.2f sec (%3.2f Mb/sec) (%d Kb/block)", elapsed, ((double)bytesRead / 1024.0 / 1024.0) / elapsed, int(blockSizeInKb));
        } else {
            txt += "\nCan't open temp file";
        }
    }

    // rnd read
    {
        size_t blockSizeInKb = 16;
        clock_t start = clock();
        size_t bytesRead = read_file_rnd(fileName.c_str(), blockSizeInKb*1024);
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (bytesRead > 0) {
            txt += format("\nSuccessfully read_rnd in %3.2f sec (%3.2f Mb/sec) (%d Kb/block)", elapsed, ((double)bytesRead / 1024.0 / 1024.0) / elapsed, int(blockSizeInKb));
        } else {
            txt += "\nCan't open temp file";
        }
    }

    // rnd read
    {
        size_t blockSizeInKb = 32;
        clock_t start = clock();
        size_t bytesRead = read_file_rnd(fileName.c_str(), blockSizeInKb*1024);
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (bytesRead > 0) {
            txt += format("\nSuccessfully read_rnd in %3.2f sec (%3.2f Mb/sec) (%d Kb/block)", elapsed, ((double)bytesRead / 1024.0 / 1024.0) / elapsed, int(blockSizeInKb));
        } else {
            txt += "\nCan't open temp file";
        }
    }

    // rnd read
    {
        size_t blockSizeInKb = 64;
        clock_t start = clock();
        size_t bytesRead = read_file_rnd(fileName.c_str(), blockSizeInKb*1024);
        clock_t end = clock();
        double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        if (bytesRead > 0) {
            txt += format("\nSuccessfully read_rnd in %3.2f sec (%3.2f Mb/sec) (%d Kb/block)", elapsed, ((double)bytesRead / 1024.0 / 1024.0) / elapsed, int(blockSizeInKb));
        } else {
            txt += "\nCan't open temp file";
        }
    }


    return env->NewStringUTF(txt.c_str());
}