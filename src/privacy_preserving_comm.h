#ifndef PRIVACY_PRESERVING_COMM_H
#define PRIVACY_PRESERVING_COMM_H

#include "attribute_auth.h"
#include <string>
#include <map>

class PrivacyPreservingComm {
private:
    AttributeAuth* auth;
    std::map<std::string, int> pseudonymMap;
    unsigned long long messageCounter;

    std::string generatePseudonym(int nodeId);

public:
    PrivacyPreservingComm(AttributeAuth* authSystem);
    ~PrivacyPreservingComm();
    
    bool sendSecureMessage(int sourceId, int destId, const std::string& message);
    std::string receiveSecureMessage(int destId, const std::string& encryptedMessage);
    std::string getPseudonym(int nodeId);
    bool updatePseudonym(int nodeId);
    bool revokePseudonym(int nodeId);
};

#endif // PRIVACY_PRESERVING_COMM_H
