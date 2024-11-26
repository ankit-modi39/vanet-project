#include "attribute_auth.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

// Helper function to convert binary data to a hexadecimal string
std::string toHex(const unsigned char* data, size_t length) {
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }
    return oss.str();
}

// Constructor for AttributeKey
AttributeAuth::AttributeKey::AttributeKey(const std::string &attr) : attribute(attr) {
    std::cout << "Generating random key for attribute: " << attribute << "\n";
    if (RAND_bytes(key, sizeof(key)) != 1) {
        throw std::runtime_error("Failed to generate random key for attribute.");
    }
    std::cout << "Random key generated successfully for attribute: " << attribute
              << " | Key: " << toHex(key, sizeof(key)) << "\n";
}

// Constructor for AttributeAuth
AttributeAuth::AttributeAuth() {
    std::cout << "Initializing OpenSSL algorithms.\n";
    OpenSSL_add_all_algorithms();
}

AttributeAuth::~AttributeAuth() {
    std::cout << "Cleaning up OpenSSL.\n";
    EVP_cleanup();
}

bool AttributeAuth::generateRandomKey(unsigned char *key, int length) {
    std::cout << "Generating random key of length: " << length << " bytes.\n";
    if (RAND_bytes(key, length) == 1) {
        std::cout << "Random key generated successfully: " << toHex(key, length) << "\n";
        return true;
    } else {
        std::cerr << "Failed to generate random key.\n";
        return false;
    }
}

std::string AttributeAuth::encryptWithKey(const std::string &message, const unsigned char *key) {
    std::cout << "Encrypting message with key: " << toHex(key, 32) << "\n";
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create encryption context.\n";
        return "";
    }

    unsigned char iv[16];
    if (!generateRandomKey(iv, 16)) {
        std::cerr << "Failed to generate IV.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    std::cout << "IV generated successfully: " << toHex(iv, 16) << "\n";

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        std::cerr << "Failed to initialize encryption.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    std::vector<unsigned char> ciphertext(message.length() + EVP_MAX_BLOCK_LENGTH);
    int len1, len2;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len1,
                          (unsigned char *)message.c_str(), message.length()) != 1) {
        std::cerr << "Encryption failed during update.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len1, &len2) != 1) {
        std::cerr << "Encryption failed during finalization.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    std::string result;
    result.reserve(16 + len1 + len2);
    result.append((char *)iv, 16);
    result.append((char *)ciphertext.data(), len1 + len2);

    std::cout << "Encryption completed successfully.\n";
    std::cout << "Ciphertext: " << toHex((unsigned char*)result.data(), result.size()) << "\n";

    return result;
}

std::string AttributeAuth::decryptWithKey(const std::string &ciphertext, const unsigned char *key) {
    std::cout << "Decrypting message with key: " << toHex(key, 32) << "\n";
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Failed to create decryption context.\n";
        return "";
    }

    const unsigned char *iv = (unsigned char *)ciphertext.data();
    const unsigned char *actualCiphertext = (unsigned char *)ciphertext.data() + 16;

    int ciphertext_len = ciphertext.length() - 16;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        std::cerr << "Failed to initialize decryption.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    std::cout << "IV used for decryption: " << toHex(iv, 16) << "\n";

    std::vector<unsigned char> plaintext(ciphertext_len + EVP_MAX_BLOCK_LENGTH);
    int len1, len2;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len1, actualCiphertext, ciphertext_len) != 1) {
        std::cerr << "Decryption failed during update.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len1, &len2) != 1) {
        std::cerr << "Decryption failed during finalization.\n";
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    std::cout << "Decryption completed successfully.\n";
    std::string plaintextStr((char *)plaintext.data(), len1 + len2);
    std::cout << "Decrypted Plaintext: " << plaintextStr << "\n";

    return plaintextStr;
}

std::string AttributeAuth::decryptMessage(int nodeId, const std::string &encryptedMessage) {
    auto it = nodeCredentials.find(nodeId);
    if (it == nodeCredentials.end()) {
        std::cerr << "Node ID " << nodeId << " not found.\n";
        return "";
    }

    const unsigned char *key = it->second.masterKey;
    std::cout << "Decrypting message for Node ID: " << nodeId << "\n";
    return decryptWithKey(encryptedMessage, key);
}

std::string AttributeAuth::encryptMessage(const std::string &message,
                                          const std::vector<std::string> &attributes) {
    std::cout << "Encrypting message with attributes.\n";

    unsigned char messageKey[32];
    if (!generateRandomKey(messageKey, 32)) {
        std::cerr << "Failed to generate message key.\n";
        return "";
    }
    std::cout << "Message key generated successfully: " << toHex(messageKey, 32) << "\n";

    std::string encryptedMessage = encryptWithKey(message, messageKey);
    if (encryptedMessage.empty()) {
        std::cerr << "Failed to encrypt message.\n";
        return "";
    }

    std::string header;
    for (const auto &attr : attributes) {
        std::cout << "Encrypting message key for attribute: " << attr << "\n";
        std::string encryptedKey = encryptWithKey(
            std::string((char *)messageKey, 32),
            nodeCredentials.begin()->second.attributes[0].key // Simplified for demo
        );
        header += attr + ":" + encryptedKey + ";";
    }

    std::cout << "Message encrypted successfully with attributes.\n";
    std::cout << "Full Encrypted Message: " << header + "|" + encryptedMessage << "\n";

    return header + "|" + encryptedMessage;
}
bool AttributeAuth::generateKeys(int nodeId, const std::vector<std::string> &attributes) {
    std::cout << "Generating keys for node ID: " << nodeId << "\n";
    NodeCredentials credentials;

    // Generate master key for the node
    if (!generateRandomKey(credentials.masterKey, 32)) {
        std::cerr << "Failed to generate master key for node ID: " << nodeId << "\n";
        return false;
    }
    std::cout << "Master key generated successfully for node ID: " << nodeId << "\n";

    // Generate keys for each attribute
    for (const auto &attr : attributes) {
        std::cout << "Generating key for attribute: " << attr << "\n";
        try {
            credentials.attributes.emplace_back(attr);
            std::cout << "Key generated successfully for attribute: " << attr << "\n";
        } catch (const std::exception &e) {
            std::cerr << "Error generating key for attribute " << attr << ": " << e.what() << "\n";
            return false;
        }
    }

    // Store credentials
    nodeCredentials[nodeId] = std::move(credentials);
    std::cout << "All keys generated and stored for node ID: " << nodeId << "\n";
    return true;
}
