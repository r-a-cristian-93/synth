#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cstring>

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

    const char* inputFilename = argv[1];
    const char* outputHeaderFilename = argv[2];
    const char* outputWavFilename = argv[3];
    const char* tableName = argv[4];
    const char* defName = argv[5];

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

    // Resample to 1024 samples using Linear interpolation
    const size_t newSampleCount = 1024;
    std::vector<int32_t> newSamples(newSampleCount);
    double scale = static_cast<double>(samples.size() - 1) / (newSampleCount - 1);

    for (size_t i = 0; i < newSampleCount; ++i) {
        double x = i * scale;
        int x0 = static_cast<int>(std::floor(x));
        int x1 = x0 + 1;
        double mu = x - x0;

        // Clamp indices to the sample range
        int32_t y1 = samples[x0];
        int32_t y2 = (x1 < samples.size()) ? samples[x1] : samples.back();

        newSamples[i] = static_cast<int32_t>(linearInterpolate(y1, y2, mu));
    }

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
    headerFile << "const int16_t wav_" << tableName << "[1024] = \n{\n";

    for (size_t i = 0; i < newSamples.size(); ++i) {
        headerFile << (newSamples[i] >> 16);
        if (i < newSamples.size() - 1) {
            headerFile << ", ";
        }
        if ((i + 1) % 10 == 0) {
            headerFile << "\n";
        }
    }

    headerFile << "\n};\n\n";
    headerFile << "#endif // WAVETABLE_H\n";

    headerFile.close();

    // Write the resampled data to a new WAV file
    std::ofstream outFile(outputWavFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Could not open output WAV file: " << outputWavFilename << std::endl;
        return 1;
    }

    // Update the WAV header for the new data size
    WAVHeader newHeader = header;
    newHeader.data_size = newSampleCount * sizeof(int32_t);
    newHeader.overall_size = newHeader.data_size + sizeof(WAVHeader) - 8; // -8 for 'RIFF' and size field

    // Write the WAV header
    outFile.write(reinterpret_cast<const char*>(&newHeader), sizeof(WAVHeader));

    // Write the resampled data
    outFile.write(reinterpret_cast<const char*>(newSamples.data()), newHeader.data_size);

    outFile.close();

    std::cout << "Conversion, resampling, and new WAV file creation completed successfully!" << std::endl;
    return 0;
}
