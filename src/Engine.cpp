#include "Engine.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
const string SALT = "sofiane_tool_cpp_thingy_sofiane_sdjfdngfjdngfjdbgdjsbgjgnfsgignfsgfsgfg45f4gf8sg4f8g 848AG8454GDE58g4fa8g4sg8f4g8fs4gf8sg4sf86g4fs8gf7s84g8f4gs5f4s5g4f8sg7tr8g4f5gh7g8jgskhongsplphgsbhsg";

EncryptionCredentials Engine::generateCredentials(const string& userPassword) {
    EncryptionCredentials creds;
    creds.password = userPassword;
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    vector<uint8_t> randomBytesKey(16);
    RAND_bytes(randomBytesKey.data(), 16);
    for (uint8_t b : randomBytesKey) {
        creds.key += charset[b % (sizeof(charset) - 1)];
    }

    vector<uint8_t> randomBytesCode(4);
    RAND_bytes(randomBytesCode.data(), 4);
    uint32_t num = (randomBytesCode[0] << 24) | (randomBytesCode[1] << 16) | 
                   (randomBytesCode[2] << 8)  | randomBytesCode[3];
    uint32_t codeVal = 100000 + (num % 900000);
    creds.code = to_string(codeVal);
    return creds;
}

vector<uint8_t> Engine::deriveKey(const EncryptionCredentials& creds) {
    string combined = creds.password + ":" + creds.key + ":" + creds.code;
    vector<uint8_t> derivedKey(32);

    PKCS5_PBKDF2_HMAC(combined.c_str(), static_cast<int>(combined.length()),
                      reinterpret_cast<const unsigned char*>(SALT.c_str()), static_cast<int>(SALT.length()),
                      100000, EVP_sha256(), 32, derivedKey.data());
    return derivedKey;
}

vector<uint8_t> Engine::aesCtrProcess(const vector<uint8_t>& data, 
                                           const vector<uint8_t>& key, 
                                           const vector<uint8_t>& iv) {
    vector<uint8_t> outData(data.size());
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key.data(), iv.data());
    int outLen1 = 0;
    EVP_EncryptUpdate(ctx, outData.data(), &outLen1, data.data(), static_cast<int>(data.size()));
    int outLen2 = 0;
    EVP_EncryptFinal_ex(ctx, outData.data() + outLen1, &outLen2);
    EVP_CIPHER_CTX_free(ctx);
    return outData;
}

bool Engine::encodeMessage(const string& inputWav, 
                           const string& outputWav, 
                           const EncryptionCredentials& creds, 
                           const string& secretText) {
    ifstream inFile(inputWav, ios::binary);
    if (!inFile) return false;
    WavHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    vector<uint8_t> audioData((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    vector<uint8_t> iv(16);
    RAND_bytes(iv.data(), 16);
    vector<uint8_t> masterKey = deriveKey(creds);
    vector<uint8_t> plaintext(secretText.begin(), secretText.end());
    vector<uint8_t> ciphertext = aesCtrProcess(plaintext, masterKey, iv);

    vector<uint8_t> payload;
    payload.insert(payload.end(), iv.begin(), iv.end());
    uint32_t size = static_cast<uint32_t>(ciphertext.size());
    payload.push_back((size >> 24) & 0xFF);
    payload.push_back((size >> 16) & 0xFF);
    payload.push_back((size >> 8) & 0xFF);
    payload.push_back(size & 0xFF);

    payload.insert(payload.end(), ciphertext.begin(), ciphertext.end());

    vector<uint8_t> payloadBits;
    for (uint8_t byte : payload) {
        for (int i = 7; i >= 0; --i) {
            payloadBits.push_back((byte >> i) & 1);
        }
    }

    if (payloadBits.size() > audioData.size()) return false;

    for (size_t i = 0; i < payloadBits.size(); ++i) {
        audioData[i] = (audioData[i] & 0xFE) | payloadBits[i];
    }

    ofstream outFile(outputWav, ios::binary);
    outFile.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));
    outFile.write(reinterpret_cast<const char*>(audioData.data()), audioData.size());
    outFile.close();

    return true;
}

string Engine::decodeMessage(const string& inputWav, 
                                  const EncryptionCredentials& creds) {
    ifstream inFile(inputWav, ios::binary);
    if (!inFile) return "";

    WavHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

    vector<uint8_t> audioData((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    vector<uint8_t> extractedBytes;
    uint8_t currentByte = 0;
    int bitCount = 0;

    for (uint8_t sample : audioData) {
        currentByte = (currentByte << 1) | (sample & 1);
        bitCount++;
        if (bitCount == 8) {
            extractedBytes.push_back(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }

    if (extractedBytes.size() < 20) return "";

    vector<uint8_t> iv(extractedBytes.begin(), extractedBytes.begin() + 16);
    
    uint32_t msgSize = (static_cast<uint32_t>(extractedBytes[16]) << 24) |
                       (static_cast<uint32_t>(extractedBytes[17]) << 16) |
                       (static_cast<uint32_t>(extractedBytes[18]) << 8)  |
                       static_cast<uint32_t>(extractedBytes[19]);

    if (20 + msgSize > extractedBytes.size()) return "[[ Corrupted Data Stream ]]";

    vector<uint8_t> ciphertext(extractedBytes.begin() + 20, extractedBytes.begin() + 20 + msgSize);
    vector<uint8_t> masterKey = deriveKey(creds);
    vector<uint8_t> decrypted = aesCtrProcess(ciphertext, masterKey, iv);

    return string(decrypted.begin(), decrypted.end());
}