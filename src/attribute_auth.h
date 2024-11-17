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
    
    // Private helper functions
    bool generateRandomKey(unsigned char* key, int length);
    std::string encryptWithKey(const std::string& message, const unsigned char* key);
    std::string decryptWithKey(const std::string& ciphertext, const unsigned char* key);

public:
    AttributeAuth();
    ~AttributeAuth();
    
    // Key management functions
    bool generateKeys(int nodeId, const std::vector<std::string>& attributes);
    bool addAttribute(int nodeId, const std::string& attribute);
    bool removeAttribute(int nodeId, const std::string& attribute);
    
    // Authentication functions
    bool verifyAttributes(int nodeId, const std::vector<std::string>& requiredAttributes);
    std::string generateAuthToken(int nodeId);
    bool verifyAuthToken(int nodeId, const std::string& token);
    
    // Message handling functions
    std::string encryptMessage(const std::string& message, const std::vector<std::string>& attributes);
    std::string decryptMessage(int nodeId, const std::string& encryptedMessage);
};

class PrivacyPreservingComm {
private:
    AttributeAuth* auth;
    std::map<std::string, int> pseudonymMap;
    unsigned long long messageCounter;
    
    std::string generatePseudonym(int nodeId);
    bool verifyMessageIntegrity(const std::string& message, const std::string& signature);

public:
    PrivacyPreservingComm(AttributeAuth* authSystem);
    ~PrivacyPreservingComm();
    
    // Communication functions
    bool sendSecureMessage(int sourceId, int destId, const std::string& message);
    std::string receiveSecureMessage(int destId, const std::string& encryptedMessage);
    
    // Privacy management functions
    std::string getPseudonym(int nodeId);
    bool updatePseudonym(int nodeId);
    bool revokePseudonym(int nodeId);
};

#endif
