#include "attribute_auth.h"
#include "privacy_preserving_comm.h"
#include <iostream>
#include <vector>

int main() {
    // Initialize AttributeAuth
    AttributeAuth authSystem;

    // Generate keys for a sample node
    int nodeId = 1;
    std::vector<std::string> attributes = {"vehicle", "trusted"};
    if (!authSystem.generateKeys(nodeId, attributes)) {
        std::cerr << "Failed to generate keys for node " << nodeId << std::endl;
        return 1;
    }

    // Initialize PrivacyPreservingComm
    PrivacyPreservingComm comm(&authSystem);

    // Use getPseudonym to get or generate a pseudonym
    std::string pseudonym = comm.getPseudonym(nodeId);
    std::cout << "Generated pseudonym: " << pseudonym << std::endl;

    // Send a secure message
    int destId = 2;
    std::string message = "Hello from Node 1 to Node 2!";
    if (comm.sendSecureMessage(nodeId, destId, message)) {
        std::cout << "Message sent successfully." << std::endl;
    } else {
        std::cerr << "Failed to send message." << std::endl;
    }

    return 0;
}
