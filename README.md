
# Privacy-Preserving Attribute-Based Authenticated Key Management Scheme

This repository provides an overview, implementation notes, and references for the **Privacy-Preserving Attribute-Based Authenticated Key Management Scheme for Accountable Vehicular Communications** as proposed in the paper by Haowen Tan, Wenying Zheng, Yunguo Guan, and Rongxing Lu. 

## Overview

Modern vehicular networks, or **Vehicular Ad Hoc Networks (VANETs)**, play a pivotal role in enabling Intelligent Transportation Systems (ITS). However, existing authentication and key management systems face challenges in balancing:
- **Privacy Preservation**
- **Accountability**
- **Dynamic Access Control**

This project implements a privacy-preserving and accountable scheme using **attribute-based cryptography** to secure vehicular communications. Key features include:
- Dynamic session identities for vehicles and roadside units (RSUs).
- Secure group key distribution using **Linear Integer Secret Sharing (LISS)**.
- Exclusive vehicle-to-vehicle (V2V) communication with accountability.

---

## Features

1. **Privacy Preservation**
   - Ensures anonymity of vehicles and unlinkability of messages across sessions.
   - Protects vehicular data from third-party surveillance.

2. **Attribute-Based Authentication**
   - Vehicles are authenticated based on predefined attributes, such as VIN, region, and driver ID.
   - Flexible access policies adapt to dynamic network topologies.

3. **Group Key Distribution**
   - Implements a secure group key-sharing mechanism using linear secret sharing.
   - Efficient batch processing supports authentication of multiple vehicles simultaneously.

4. **Accountability**
   - Incorporates hash chains for tracking sent and received messages.
   - Non-repudiation ensures vehicles cannot deny their communication actions.

5. **Security Against Attacks**
   - Resistant to replay attacks, forgery, and collusion.
   - Conditional privacy allows identity revelation of malicious entities.

---

## System Model

The system comprises three main components:
1. **Trusted Authority (TA):**
   - Centralized authority for registration, key issuance, and auditing.
2. **Roadside Units (RSUs):**
   - Edge devices handling local vehicle authentication and communication.
3. **Vehicles:**
   - Mobile nodes equipped with onboard units (OBUs) for secure data transmission.

---

## Methodology

The scheme consists of three phases:

### 1. Vehicle Authentication
- Vehicles register with the Trusted Authority (TA) using their attributes.
- Dynamic session identities are generated to ensure privacy.
- Batch authentication accelerates the validation process.

### 2. Attribute-Based Group Key Distribution
- RSUs securely distribute group keys to authenticated vehicles using LISS.
- Access policies are dynamically created based on vehicle attributes.

### 3. Accountable V2V Communication
- Homomorphic encryption ensures secure peer-to-peer communication.
- Metadata is uploaded to RSUs and TA for auditing.
- Hash chains record message logs, ensuring accountability.

---

## Security Analysis

The scheme ensures:
- **Anonymity:** Prevents tracking of vehicles by dynamic session identities.
- **Replay Attack Resistance:** Timestamp validation ensures message freshness.
- **Unlinkability:** Messages from the same vehicle cannot be associated across sessions.
- **Conditional Privacy:** Malicious users can be identified if necessary.
- **Session Key Establishment:** Secure keys for V2V and V2R interactions.

---

## Advantages

1. **Efficiency:** Batch processing reduces authentication overhead.
2. **Scalability:** Supports dense vehicular networks in urban areas.
3. **Accountability:** Ensures non-repudiation and secure auditing.
4. **Privacy Preservation:** Protects user data against eavesdropping and tampering.

---

## Installation & Usage

This project is a conceptual implementation of the proposed scheme. Follow these steps to use this repository:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-username/privacy-preserving-vehicular-comm.git
   bash scripts/run_simulation.sh
