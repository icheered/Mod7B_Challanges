/**
 * RoutingChallengeClient.cpp
 *
 *   Version: 2018-03-12
 *    Author: Jaco ter Braak & Frans van Dijk, University of Twente.
 * Copyright: University of Twente, 2015-2019
 *
 ************************************************************
 *                                                          *
 **                 DO NOT EDIT THIS FILE                  **
 ***         YOU DO NOT HAVE TO LOOK IN THIS FILE         ***
 **** IF YOU HAVE QUESTIONS PLEASE DO ASK A TA FOR HELP  ****
 *****                                                  *****
 ************************************************************
 */

#include "RoutingChallengeClient.h"
#include "IRoutingProtocol.h"

namespace framework {

    RoutingChallengeClient::RoutingChallengeClient(std::string serverAddress,
        int32_t serverPort, std::string clientGroupKey) {
        if (clientGroupKey == "get-your-token-from-the-website") {
            std::cerr << "[FRAMEWORK ERROR] Please set your group token in Program.cpp" << std::endl;
            exit(EXIT_FAILURE);
        }

        this->host = serverAddress;
        std::ostringstream ss;
        ss << serverPort;
        this->port = ss.str();
        this->groupKey = clientGroupKey;

        this->sock = 0;

        this->clientConnect();

    }

    RoutingChallengeClient::~RoutingChallengeClient() {

    }

    void RoutingChallengeClient::setRoutingProtocolAndTock(IRoutingProtocol *protocol) {
        this->protocol = protocol;
        sendControlMessage("TOCK");

    }

    /**
     * Requests a simulation start from the server
     */
    void RoutingChallengeClient::RequestStart() {
        if (this->simulationState == Idle) {
            sendControlMessage("START");
        }
    }

    /**
     * Starts the simulation
     */
    void RoutingChallengeClient::Start() {
        if (this->simulationState == Idle) {
            this->simulationState = Started;
        }
    }

    /**
     * Handles communication between the server and the protocol implementation
     */
    void RoutingChallengeClient::run() {
        bool stopThread = false;
        while (!stopThread && this->simulationState != Finished) {
            std::string message = this->getControlMessage();
            if (message != "") {
                std::vector<std::string> splitMessage = this->split(message, ' ');
                if (message.find("FAIL") == 0) {
                    if (splitMessage.size() > 1) {
                        std::cerr << "[SERVER MESSAGE] Failure: "
                            << message.substr(message.find(' ') + 1);
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                        exit(EXIT_FAILURE);
                    }
                    clearControlMessage();
                    Stop();
                }
                else if (splitMessage.size() > 0 && splitMessage[0].find("INFO") == 0) {
                    std::cerr << "[SERVER MESSAGE] Info: " << message.substr(message.find(' ') + 1) << std::endl;
                    clearControlMessage();
                }
                else if (message.find("START") == 0) {
                    Start();
                    clearControlMessage();
                }
                else if (message.find("INITTEST") == 0) {
                    this->packetBuffer.clear();
                    this->simulationState = Started;
                    if (splitMessage.size() > 1) {
                        int32_t testID;
                        if (!(std::stringstream(splitMessage[1]) >> testID)) {
                            std::cerr << "[FRAMEWORK ERROR] Error parsing int testID. Input was: "
                                << splitMessage[1] << std::endl;
                            this->testID++;
                        }
                        else {
                            this->testID = testID;
                        }
                    }
                    clearControlMessage();
                }
                else if (message.find("ADDRESS") == 0) {
                    if (splitMessage.size() > 1) {
                        int32_t addr;
                        if (!(std::stringstream(splitMessage[1]) >> addr)) {
                            std::cerr << "[FRAMEWORK ERROR] Error parsing int address. Input was: "
                                << splitMessage[1] << std::endl;
                            std::this_thread::sleep_for(std::chrono::seconds(10));
                            exit(EXIT_FAILURE);
                        }
                        this->address = addr;
                    }
                    clearControlMessage();
                }
                else if (message.find("RUNTEST") == 0) {
                    this->simulationState = TestRunning;
                    clearControlMessage();
                }
                else if (message.find("TESTCOMPLETE") == 0) {
                    this->simulationState = TestComplete;
                    clearControlMessage();
                }
                else if (message.find("TABLE") == 0) {
                    if (splitMessage.size() > 1) {
                        if (splitMessage[1].find("REQUEST") == 0) {
                            UploadForwardingTable();
                        }
                        else if (splitMessage[1].find("FAIL") == 0) {
                            std::cerr << "[SERVER ERROR] Failed to upload forwarding table"
                                << std::endl;
                            std::this_thread::sleep_for(std::chrono::seconds(10));
                            exit(EXIT_FAILURE);
                        }
                    }
                    clearControlMessage();
                }
                else if (message.find("TRANSMIT") == 0
                    && this->IsSimulationRunning()) {
                    if (splitMessage[1].find("DISCONNECTED") == 0) {
                        if (splitMessage.size() == 3) {
                            // Disconnected interface
                            int32_t destination;
                            if (!(std::stringstream(splitMessage[2]) >> destination)) {
                                std::cerr
                                    << "[FRAMEWORK ERROR] Error parsing int destination. Input was: "
                                    << splitMessage[2] << std::endl;
                                std::this_thread::sleep_for(std::chrono::seconds(10));
                                exit(EXIT_FAILURE);
                            }
                            clearControlMessage();
                        }

                    }
                    else {
                        if (splitMessage.size() == 3) {
                            int32_t cost;
                            if (!(std::stringstream(splitMessage[1]) >> cost)) {
                                std::cerr << "[FRAMEWORK ERROR] Error parsing int cost. Input was: "
                                    << splitMessage[1] << std::endl;
                                std::this_thread::sleep_for(std::chrono::seconds(10));
                                exit(EXIT_FAILURE);
                            }

                            std::vector<unsigned char> packetData = base64_decode(splitMessage[2]);

                            int32_t srcAddr = (packetData[0] << 24)
                                + (packetData[1] << 16)
                                + (packetData[2] << 8)
                                + packetData[3];
                            int32_t dstAddr = (packetData[4] << 24)
                                + (packetData[5] << 16)
                                + (packetData[6] << 8)
                                + packetData[7];

                            if (packetData[8] == 0) {
                                int32_t dataLength = (packetData[9] << 24)
                                    + (packetData[10] << 16)
                                    + (packetData[11] << 8)
                                    + packetData[12];

                                std::vector<unsigned char>::const_iterator dataBegin =
                                    packetData.begin() + 13;
                                std::vector<unsigned char>::const_iterator dataEnd =
                                    packetData.begin() + 13 + dataLength;
                                std::vector<unsigned char> data(dataBegin, dataEnd);

                                Packet *packet = new Packet(srcAddr, dstAddr, data); // memory leak?!

                                packetBuffer.push_back(PacketWithLinkCost{ *packet, cost });
                            }
                            else if (packetData[8] == 1) {
                                uint32_t nRows = (packetData[9] << 24)
                                    + (packetData[10] << 16)
                                    + (packetData[11] << 8)
                                    + packetData[12];
                                uint32_t nColumns = (packetData[13] << 24)
                                    + (packetData[14] << 16)
                                    + (packetData[15] << 8)
                                    + packetData[16];

                                // parse packet data
//                        DataTable dataTable(nColumns);
                                DataTable *dataTable = new DataTable(nColumns);   // memory leak?!
                                for (uint32_t i = 0; i < nRows; i++) {
                                    for (uint32_t j = 0; j < nColumns; j++) {
                                        int32_t cellData =
                                            ((uint8_t)packetData[17 + 4 * i * nColumns + 4 * j] << 24)
                                            + ((uint8_t)packetData[18 + 4 * i * nColumns + 4 * j] << 16)
                                            + ((uint8_t)packetData[19 + 4 * i * nColumns + 4 * j] << 8)
                                            + (uint8_t)packetData[20 + 4 * i * nColumns + 4 * j];

                                        dataTable->set(i, j, cellData);
                                    }
                                }
                                //std::cout<< nRows <<  " " << nColumns << " " << dataTable->get(0,0) << "\n";

                                                                // instantiate the packet with the provided data
                                //                        Packet packet(srcAddr, dstAddr, &dataTable);
                                Packet *packet = new Packet(srcAddr, dstAddr, dataTable); // memory leak?!

                                packetBuffer.push_back(PacketWithLinkCost{ *packet, cost });
                            }
                            else {
                                std::cerr << "[FRAMEWORK ERROR] Could not parse packet." << std::endl;
                            }

                            clearControlMessage();
                        }
                    }
                }
                else if (message.find("TICK") == 0) {
                    protocol->tick(packetBuffer);
                    packetBuffer.clear();
                    TockWithTable();
                    clearControlMessage();
                }
                else if (message.find("FINISH") == 0
                    || message.find("CLOSED") == 0) {
                    if (splitMessage.size() > 1) {
                        std::cerr << "[SERVER MESSAGE] Simulation aborted because: "
                            << message.substr(message.find(" ") + 1)
                            << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                        exit(EXIT_FAILURE);
                    }
                    std::cout << "[FRAMEWORK] Stopping..." << std::endl;
                    this->simulationState = Finished;
                    clearControlMessage();
                }
                else if (message.find("NO-OP") == 0) {
                    clearControlMessage();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    /**
     * Stops the client, and disconnects it from the server.
     */
    void RoutingChallengeClient::Stop() {
        this->simulationState = Finished;

        this->eventLoopThread.join();

        shutdown(this->sock, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#ifdef _MSC_VER
        closesocket(this->sock);
#else
        close(this->sock);
#endif 
    }

    /**
     * @return whether the simulation has been started
     */
    bool RoutingChallengeClient::IsSimulationRunning() {
        return (this->simulationState == Started
            || this->simulationState == TestRunning
            || this->simulationState == TestComplete);

    }

    int32_t RoutingChallengeClient::getAddress() {
        return this->address;
    }

    SimulationState RoutingChallengeClient::getSimulationState() {
        return this->simulationState;
    }

    int32_t RoutingChallengeClient::getTestID() {
        return this->testID;
    }

    /**
     * Transmits a packet to another node
     *
     * @param packet
     *            Packet contains source address, destination address, and data.
     *            Destination address may be 0 to transmit to all neighbours
     *            (broadcast).
     */
    void RoutingChallengeClient::Transmit(Packet packet) {
        if (!this->IsSimulationRunning()) {
            return; // Silent ignore.
        }

        std::vector<unsigned char> packetHeader = std::vector<unsigned char>(9);

        int32_t srcAddr = packet.getSourceAddress();
        packetHeader[0] = (unsigned char)(srcAddr >> 24);
        packetHeader[1] = (unsigned char)(srcAddr >> 16);
        packetHeader[2] = (unsigned char)(srcAddr >> 8);
        packetHeader[3] = (unsigned char)(srcAddr >> 0);

        int32_t dstAddr = packet.getDestinationAddress();
        packetHeader[4] = (unsigned char)(dstAddr >> 24);
        packetHeader[5] = (unsigned char)(dstAddr >> 16);
        packetHeader[6] = (unsigned char)(dstAddr >> 8);
        packetHeader[7] = (unsigned char)(dstAddr >> 0);

        std::vector<unsigned char> packetData;

        if (packet.isRaw()) {
            packetHeader[8] = 0;

            std::vector<unsigned char> rawData = packet.getRawData();
            int32_t dataLength = (int32_t)rawData.size();

            packetData = std::vector<unsigned char>(4);

            packetData[0] = (unsigned char)(dataLength >> 24);
            packetData[1] = (unsigned char)(dataLength >> 16);
            packetData[2] = (unsigned char)(dataLength >> 8);
            packetData[3] = (unsigned char)(dataLength >> 0);

            packetData.insert(packetData.end(), rawData.begin(), rawData.end());
        }
        else {
            packetHeader[8] = 1;

            // Serialize the table
            DataTable dataTable = *packet.getDataTable();

            packetData = std::vector<unsigned char>(8 + 4 * dataTable.getNRows() * dataTable.getNColumns());

            uint32_t nRows = dataTable.getNRows();
            packetData[0] = (unsigned char)(nRows >> 24);
            packetData[1] = (unsigned char)(nRows >> 16);
            packetData[2] = (unsigned char)(nRows >> 8);
            packetData[3] = (unsigned char)(nRows >> 0);

            uint32_t nColumns = dataTable.getNColumns();
            packetData[4] = (unsigned char)(nColumns >> 24);
            packetData[5] = (unsigned char)(nColumns >> 16);
            packetData[6] = (unsigned char)(nColumns >> 8);
            packetData[7] = (unsigned char)(nColumns >> 0);

            for (uint32_t i = 0; i < dataTable.getNRows(); i++) {
                for (uint32_t j = 0; j < dataTable.getNColumns(); j++) {

                    int32_t cellData = dataTable.get(i, j);
                    packetData[8 + i * dataTable.getNColumns() * 4 + j * 4 + 0] = (unsigned char)(cellData >> 24);
                    packetData[8 + i * dataTable.getNColumns() * 4 + j * 4 + 1] = (unsigned char)(cellData >> 16);
                    packetData[8 + i * dataTable.getNColumns() * 4 + j * 4 + 2] = (unsigned char)(cellData >> 8);
                    packetData[8 + i * dataTable.getNColumns() * 4 + j * 4 + 3] = (unsigned char)(cellData >> 0);
                }
            }
        }

        std::vector<unsigned char> packetBytes;
        packetBytes.insert(packetBytes.begin(), packetHeader.begin(), packetHeader.end());
        packetBytes.insert(packetBytes.end(), packetData.begin(), packetData.end());

        sendControlMessage(
            "TRANSMIT "
            + base64_encode(
                const_cast<const unsigned char *>(packetBytes.data()),
                packetBytes.size()));
    }

    void RoutingChallengeClient::clientConnect() {
        struct addrinfo hints;
        struct addrinfo *result, *rp;
        ssize_t s;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = 0;
        hints.ai_protocol = IPPROTO_TCP;

        s = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
        if (s != 0) {
            std::cerr << "[FRAMEWORK ERROR] getaddrinfo:" << gai_strerror(s) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(EXIT_FAILURE);
        }

        for (rp = result; rp != NULL; rp = rp->ai_next) {
            sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sock == -1)
                continue;

            if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
                break; /* Success */

#ifdef _MSC_VER
            closesocket(sock);
#else
            close(sock);
#endif 
        }

        if (rp == NULL) { /* No address succeeded */
            std::cerr << "[FRAMEWORK ERROR] Could not connect" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(EXIT_FAILURE);
        }

        freeaddrinfo(result);

        // set non-blocking socket
#ifdef _MSC_VER
        unsigned long valueEnable = 1;
        ioctlsocket(sock, FIONBIO, &valueEnable);
#else
        ssize_t x;
        x = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, x | O_NONBLOCK);
#endif

        // expect hello message
        if (this->getControlMessageBlocking() != "REGISTER") {
            std::cerr << "[FRAMEWORK ERROR] Did not get expected hello from server" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(EXIT_FAILURE);
        }
        this->clearControlMessage();

        // register
        this->sendControlMessage("REGISTER " + this->groupKey);

        std::string reply = getControlMessageBlocking();
        if (reply != "OK") {
            std::string reason = reply.substr(reply.find(' ') + 1);
            std::cerr << "[FRAMEWORK ERROR] Could not register with server: " << reason << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(EXIT_FAILURE);
        }
        clearControlMessage();

        std::cout << "[FRAMEWORK] thread" << std::endl;
        this->eventLoopThread = std::thread(&RoutingChallengeClient::run, this);
    }

    void RoutingChallengeClient::UploadForwardingTable() {
        std::map<int32_t, int32_t> table = this->protocol->getForwardingTable();
        std::vector<unsigned char> tableData = SerializeTable(table);

        sendControlMessage("TABLE " + base64_encode(tableData.data(), tableData.size()));
    }

    void RoutingChallengeClient::TockWithTable() {
        std::map<int32_t, int32_t> table = this->protocol->getForwardingTable();
        std::vector<unsigned char> tableData = SerializeTable(table);

        sendControlMessage("TOCK " + base64_encode(tableData.data(), tableData.size()));
    }

    std::vector<unsigned char> RoutingChallengeClient::SerializeTable(std::map<int32_t, int32_t> table) {
        std::vector<unsigned char> serialized = std::vector<unsigned char>();

        for (std::map<int32_t, int32_t>::iterator it = table.begin(); it != table.end(); it++) {
            int32_t destination = it->first;
            int32_t nextHop = it->second;
            serialized.push_back((unsigned char)(destination >> 24));
            serialized.push_back((unsigned char)(destination >> 16));
            serialized.push_back((unsigned char)(destination >> 8));
            serialized.push_back((unsigned char)(destination >> 0));
            serialized.push_back((unsigned char)(nextHop >> 24));
            serialized.push_back((unsigned char)(nextHop >> 16));
            serialized.push_back((unsigned char)(nextHop >> 8));
            serialized.push_back((unsigned char)(nextHop >> 0));
        }
        return serialized;
    }

    std::string RoutingChallengeClient::getControlMessageBlocking() {
        std::string message = "";
        while (message == "") {
            message = this->getControlMessage();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return message;
    }

    std::string RoutingChallengeClient::getControlMessage() {
        char socketBuffer[BUF_SIZE];
#ifdef _MSC_VER
        ssize_t nread = recv(sock, socketBuffer, BUF_SIZE, 0);
#else
        ssize_t nread = read(sock, socketBuffer, BUF_SIZE);
#endif
        if (nread != -1) {
            socketBufferStr.append(socketBuffer, nread);
        }
        if (this->currentControlMessage == "") {
            ssize_t pos = socketBufferStr.find('\n');
            if (pos > 0) {
                std::string line = socketBufferStr.substr(0, pos);
                if (line.length() > this->protocolString.length() + 1
                    && line.substr(0, this->protocolString.length())
                    == this->protocolString) {
                    this->currentControlMessage = line.substr(
                        this->protocolString.length() + 1,
                        line.length() - (this->protocolString.length() + 1));
                    socketBufferStr = socketBufferStr.substr(pos + 1,
                        socketBufferStr.length() - (pos + 1));
                }
                else {
                    std::cerr << "[FRAMEWORK ERROR] Protocol mismatch with server" << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                    exit(EXIT_FAILURE);
                }
            }
        }

        return this->currentControlMessage;
    }

    void RoutingChallengeClient::clearControlMessage() {
        this->currentControlMessage = "";
    }

    void RoutingChallengeClient::sendControlMessage(std::string message) {
        std::string controlMessage = this->protocolString + " " + message + "\n";
#ifdef _MSC_VER
        ssize_t nwritten = send(sock, controlMessage.c_str(), controlMessage.length(), 0);
#else
        ssize_t nwritten = write(sock, controlMessage.c_str(), controlMessage.length());
#endif
        if (nwritten != (ssize_t)controlMessage.length()) {
            std::cerr << "[FRAMEWORK ERROR] Write failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(EXIT_FAILURE);
        }
    }

    std::vector<std::string> &RoutingChallengeClient::split(const std::string &s,
        char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> RoutingChallengeClient::split(const std::string &s,
        char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }

} /* namespace framework */
