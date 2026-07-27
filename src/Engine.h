#ifndef ENGINE_H
#define ENGINE_H

#include<bits/stdc++.h>
using namespace std;

struct WavHeader {
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2ID[4];
    uint32_t subchunk2Size;
};

struct EncryptionCredentials {
    string password;
    string key;
    string code;
};

class Engine {
public:
    static EncryptionCredentials generateCredentials(const string& userPassword);
    static bool encodeMessage(const string& inputWav, 
                             const string& outputWav, 
                             const EncryptionCredentials& creds, 
                             const string& secretText);
    static string decodeMessage(const string& inputWav, 
                                     const EncryptionCredentials& creds);
private:
    static vector<uint8_t> deriveKey(const EncryptionCredentials& creds);
    static vector<uint8_t> aesCtrProcess(const vector<uint8_t>& data, 
                                             const vector<uint8_t>& key, 
                                             const vector<uint8_t>& iv);
};

#endif // ENGINE_H