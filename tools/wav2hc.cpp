#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdlib>

// Structure to hold the WAV file header
struct WAVHeader {
    char riff[4];
    uint32_t overall_size;
    char wave[4];
    char fmt_chunk_marker[4];
    uint32_t length_of_fmt;
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byterate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_chunk_header[4];
    uint32_t data_size;
};

// Linear interpolation function
double linearInterpolate(double y1, double y2, double mu) {
    return y1 * (1 - mu) + y2 * mu;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <input wav file> <output header file> <output wav file>" << std::endl;
        return 1;
    }

    const int bitDepth = atoi(argv[1]);
    const char* inputFilename = argv[2];
    const char* outputHeaderFilename = argv[3];
    const char* outputWavFilename = argv[4];
    const char* tableName = argv[5];
    const char* defName = argv[6];

    // Open the input WAV file
    std::ifstream wavFile(inputFilename, std::ios::binary);
    if (!wavFile) {
        std::cerr << "Could not open input WAV file: " << inputFilename << std::endl;
        return 1;
    }

    // Read the WAV file header
    WAVHeader header;
    wavFile.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    if (std::strncmp(header.riff, "RIFF", 4) != 0 || std::strncmp(header.wave, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV file format" << std::endl;
        return 1;
    }

    // Verify that the input file is 32-bit signed PCM
    if (header.bits_per_sample != 32 || header.format_type != 1) {
        std::cerr << "Unsupported WAV file format. Expected 32-bit signed PCM." << std::endl;
        return 1;
    }

    // Read the sample data
    std::vector<int32_t> samples(header.data_size / sizeof(int32_t));
    wavFile.read(reinterpret_cast<char*>(samples.data()), header.data_size);
    wavFile.close();

    // Open the output header file
    std::ofstream headerFile(outputHeaderFilename);
    if (!headerFile) {
        std::cerr << "Could not open output header file: " << outputHeaderFilename << std::endl;
        return 1;
    }

    // Write the header file contents
    headerFile << "#ifndef WAV_" << defName << "_H\n";
    headerFile << "#define WAV_" << defName << "_H\n\n";
    headerFile << "#include <cstdint>\n\n";
    headerFile << "const int16_t wav_" << tableName << "_length = " << samples.size() << ";\n";
    headerFile << "const int16_t wav_" << tableName << "[" << samples.size() << "] = \n{\n";

    for (size_t i = 0; i < samples.size(); ++i) {
        headerFile << (samples[i] >> (32 - bitDepth) );
        if (i < samples.size() - 1) {
            headerFile << ", ";
        }
        if ((i + 1) % 10 == 0) {
            headerFile << "\n";
        }
    }

    headerFile << "\n};\n\n";
    headerFile << "#endif // WAVETABLE_H\n";

    headerFile.close();

    std::cout << "Conversion completed successfully!" << std::endl;
    return 0;
}
