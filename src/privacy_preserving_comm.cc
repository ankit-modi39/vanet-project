#include "attribute_auth.h"
#include <chrono>
#include <random>

PrivacyPreservingComm::PrivacyPreservingComm(AttributeAuth* authSystem) 
    : auth(authSystem), messageCounter(0) {
}

PrivacyPreservingComm::~PrivacyPreservingComm() {
}

std::string PrivacyPreservingComm::generatePseudonym(int nodeId) {
    // Generate a random pseudonym
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hex_chars = "0123456789abcdef";
    std::string pseudonym;
    for (int i = 0; i < 32; ++i) {
        pseudonym += hex_chars[dis(gen)];
    }
    
    // Map pseudonym to nodeId
    pseudonymMap[pseudonym] = nodeId;
    return pseudonym;
}

std::string PrivacyPreservingComm::getPseudonym(int nodeId) {
    // Check if pseudonym already exists for the given nodeId
    for (const auto& entry : pseudonymMap) {
        if (entry.second == nodeId) {
            return entry.first;
        }
    }
    // If no pseudonym exists, generate a new one
    return generatePseudonym(nodeId);
}

bool PrivacyPreservingComm::sendSecureMessage(int sourceId, int destId, 
                                             const std::string& message) {
    // Get source pseudonym
    std::string sourcePseudonym = getPseudonym(sourceId);
    if (sourcePseudonym.empty()) {
        sourcePseudonym = generatePseudonym(sourceId);
    }

    // Prepare message structure
    messageCounter++;
    std::string messageStructure = 
        sourcePseudonym + "|" + 
        std::to_string(messageCounter) + "|" + 
        message;

    // Encrypt message using attribute-based encryption
    std::vector<std::string> attributes = {"vehicle", "authorized"};  // Example attributes
    std::string encryptedMessage = auth->encryptMessage(messageStructure, attributes);
    
    if (encryptedMessage.empty()) {
        // Encryption failed (e.g., attribute mismatch)
        std::cerr << "Encryption failed for message from " 
                  << sourceId << " to " << destId << "\n";
        return false;
    }

    // Simulate real-world transmission success/failure
    double transmissionProbability = 0.9; // 90% chance of successful transmission
    double randomFactor = static_cast<double>(rand()) / RAND_MAX;
    if (randomFactor > transmissionProbability) {
        // Simulate transmission failure
        std::cerr << "Message transmission failed from " 
                  << sourceId << " to " << destId << "\n";
        return false;
    }

    // Simulate network delay
    int delay = rand() % 200; // Random delay up to 200ms
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    // Transmission successful
    std::cout << "Message successfully transmitted from " 
              << sourceId << " to " << destId << " with delay: " 
              << delay << " ms\n";
    return true;
}


std::string PrivacyPreservingComm::receiveSecureMessage(int destId, 
                                                       const std::string& encryptedMessage) {
    // Decrypt message
    std::string decryptedMessage = auth->decryptMessage(destId, encryptedMessage);
    if (decryptedMessage.empty()) {
        return "";
    }
    
    // Parse message components
    size_t firstDelim = decryptedMessage.find('|');
    size_t secondDelim = decryptedMessage.find('|', firstDelim + 1);
    
    if (firstDelim == std::string::npos || secondDelim == std::string::npos) {
        return "";
    }
    
    std::string senderPseudonym = decryptedMessage.substr(0, firstDelim);
    std::string messageContent = decryptedMessage.substr(secondDelim + 1);
    
    // Verify sender pseudonym
    auto it = pseudonymMap.find(senderPseudonym);
    if (it == pseudonymMap.end()) {
        return "";
    }
    
    return messageContent;
}

bool PrivacyPreservingComm::updatePseudonym(int nodeId) {
    // Remove old pseudonym
    for (auto it = pseudonymMap.begin(); it != pseudonymMap.end(); ) {
        if (it->second == nodeId) {
            it = pseudonymMap.erase(it);
        } else {
            ++it;
        }
    }
    
    // Generate new pseudonym
    generatePseudonym(nodeId);
    return true;
}
