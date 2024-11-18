#ifndef ATTRIBUTE_AUTH_H
#define ATTRIBUTE_AUTH_H

#include <string>
#include <vector>
#include <map>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

class AttributeAuth {
private:
    struct AttributeKey {
        std::string attribute;
        unsigned char key[32]; // 256-bit key
        AttributeKey(const std::string& attr);
    };
    
    struct NodeCredentials {
        std::vector<AttributeKey> attributes;
        unsigned char masterKey[32];
        std::string pseudonym;
    };
    
    std::map<int, NodeCredentials> nodeCredentials;
    
    bool generateRandomKey(unsigned char* key, int length);
    std::string encryptWithKey(const std::string& message, const unsigned char* key);
    std::string decryptWithKey(const std::string& ciphertext, const unsigned char* key);

public:
    AttributeAuth();
    ~AttributeAuth();
    
    bool generateKeys(int nodeId, const std::vector<std::string>& attributes);
    bool addAttribute(int nodeId, const std::string& attribute);
    bool removeAttribute(int nodeId, const std::string& attribute);
    
    bool verifyAttributes(int nodeId, const std::vector<std::string>& requiredAttributes);
    std::string generateAuthToken(int nodeId);
    bool verifyAuthToken(int nodeId, const std::string& token);
    std::string encryptMessage(const std::string& message, const std::vector<std::string>& attributes);
    std::string decryptMessage(int nodeId, const std::string& encryptedMessage);
};

#endif // ATTRIBUTE_AUTH_H
