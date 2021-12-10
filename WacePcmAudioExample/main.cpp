#include <iostream>
#include <fstream>

using namespace std;

// RIFF Chunk Descriptor
const string chunk_id = "RIFF";
const string chunk_size = "----";
const string format = "WAVE";

// Format SubChunk (FMT)
const string subchunk1_id = "fmt ";
const int subchunk1_size = 16;
const int audio_format = 1;                                               // PCM = 0x0001 (Linear quantization, uncompressed)
const int num_channels = 1;                                               // 1 = mono, 2 = stereo
const int sample_rate = 44100;                                            // 8000, 16000, (44100 = Music targeted quality)
const int bits_per_sample = 16;                                           // 8, 16, 24 bit per sample (bit size)
const int byte_rate = sample_rate * num_channels * (bits_per_sample / 8); // ByteRate = SampleRate * num_channels * (BitsPerSample / 8)
const int block_align = num_channels * (bits_per_sample / 8);             // BlockAlign = num_channels * (BitsPerSample / 8)

// Data SubChunk
const string subchunk2_id = "data";
const string subchunk2_size = "----";

// additional constants for data
const int duration_secs = 2;
const int max_amplitude = 32724; // (2 ^ Subchunk1_size / 2) - header_size (44 bytes) = 32724 in this case
const int number_of_samples = num_channels * sample_rate;
const double frequency = subchunk1_size * bits_per_sample;
const double PI = 3.14159265358979323846;

// Helper Methods
void write_as_bytes(ofstream& file, int value, int byte_size)
{
    file.write((char*)&value, (double)byte_size);
}

int main()
{
    ofstream wav;
    wav.open("soundTest.wav", ios::binary);

    if (wav.is_open())
    {
        // RIFF Chunk Descriptor
        wav << chunk_id;
        wav << chunk_size;
        wav << format;

        // Format SubChunk (FMT)
        wav << subchunk1_id;
        write_as_bytes(wav, subchunk1_size, 4);
        write_as_bytes(wav, audio_format, 2);
        write_as_bytes(wav, num_channels, 2);
        write_as_bytes(wav, sample_rate, 4);
        write_as_bytes(wav, byte_rate, 4);
        write_as_bytes(wav, block_align, 2);
        write_as_bytes(wav, bits_per_sample, 2);

        // Data SubChunk
        wav << subchunk2_id;
        wav << subchunk2_size;

        // get data position in file
        int start_data_position = wav.tellp();

        for (int i = 0; i < number_of_samples; i++)
        {
            // we must not exceed the max amplitude of 88200 defined previously
            // set the single generated to operate as a wave
            double amplitude = (double)i / sample_rate * max_amplitude;
            double value = sin((2 * PI * i * frequency) / sample_rate);

            double channel1 = amplitude * value / 2;                   // fat channel from high (left speaker)
           double channel2 = (max_amplitude - amplitude) * value; // slim channel to low (right speaker)

            // write to  wave file as binary 2 bytes each
            write_as_bytes(wav, channel1, 2);
            write_as_bytes(wav, channel2, 2);
        }

        // get data position in file
        int end_data_position = wav.tellp();

        // filter data position by removing 4 bytes of data added earlier
        wav.seekp(start_data_position - 4);

        write_as_bytes(wav, end_data_position - start_data_position, 4);

        wav.seekp(4, ios::beg);

        write_as_bytes(wav, end_data_position - 8, 4);
    }

    wav.close();

    return 0;
}