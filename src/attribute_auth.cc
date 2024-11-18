#include "attribute_auth.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

AttributeAuth::AttributeKey::AttributeKey(const std::string &attr) : attribute(attr)
{
    // Generate random key for this attribute
    RAND_bytes(key, sizeof(key));
}

AttributeAuth::AttributeAuth()
{
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
}

AttributeAuth::~AttributeAuth()
{
    EVP_cleanup();
}

bool AttributeAuth::generateRandomKey(unsigned char *key, int length)
{
    return RAND_bytes(key, length) == 1;
}

bool AttributeAuth::generateKeys(int nodeId, const std::vector<std::string> &attributes)
{
    NodeCredentials credentials;

    // Generate master key for the node
    if (!generateRandomKey(credentials.masterKey, 32))
    {
        return false;
    }

    // Generate keys for each attribute
    for (const auto &attr : attributes)
    {
        credentials.attributes.emplace_back(attr);
    }

    // Store credentials
    nodeCredentials[nodeId] = std::move(credentials);
    return true;
}

std::string AttributeAuth::encryptWithKey(const std::string &message, const unsigned char *key)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        return "";
    }

    unsigned char iv[16];
    if (!generateRandomKey(iv, 16))
    {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    std::vector<unsigned char> ciphertext(message.length() + EVP_MAX_BLOCK_LENGTH);
    int len1, len2;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len1,
                          (unsigned char *)message.c_str(), message.length()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len1, &len2) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);

    // Combine IV and ciphertext
    std::string result;
    result.reserve(16 + len1 + len2);
    result.append((char *)iv, 16);
    result.append((char *)ciphertext.data(), len1 + len2);

    return result;
}

bool AttributeAuth::verifyAttributes(int nodeId, const std::vector<std::string> &requiredAttributes)
{
    auto it = nodeCredentials.find(nodeId);
    if (it == nodeCredentials.end())
    {
        return false;
    }

    // Check if node has all required attributes
    for (const auto &reqAttr : requiredAttributes)
    {
        bool found = false;
        for (const auto &nodeAttr : it->second.attributes)
        {
            if (nodeAttr.attribute == reqAttr)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    return true;
}

std::string AttributeAuth::encryptMessage(const std::string &message,
                                          const std::vector<std::string> &attributes)
{
    // Generate message key
    unsigned char messageKey[32];
    if (!generateRandomKey(messageKey, 32))
    {
        return "";
    }

    // Encrypt message with message key
    std::string encryptedMessage = encryptWithKey(message, messageKey);
    if (encryptedMessage.empty())
    {
        return "";
    }

    // For each attribute, encrypt message key
    std::string header;
    for (const auto &attr : attributes)
    {
        std::string encryptedKey = encryptWithKey(
            std::string((char *)messageKey, 32),
            nodeCredentials.begin()->second.attributes[0].key // Simplified for demo
        );

        header += attr + ":" + encryptedKey + ";";
    }

    return header + "|" + encryptedMessage;
}
