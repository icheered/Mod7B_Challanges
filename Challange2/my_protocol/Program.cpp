/**
 * main.cpp
 *
 *   Version: 2019-02-13
 *    Author: Jaco ter Braak & Frans van Dijk, University of Twente.
 * Copyright: University of Twente, 2015-2019
 */

#include <iostream>
#include <string>
#include <time.h>

#ifdef _MSC_VER
#include <conio.h>
#else
#include <sys/select.h>
#endif

#include "../framework/DRDTChallengeClient.h"
#include "../framework/NetworkLayer.h"
#include "../framework/IRDTProtocol.h"
#include "MyProtocol.h"

using namespace my_protocol;

// Change to your group authentication token
std::string groupToken = "68e22b48-0892-4898-b9ae-9c447c160eab";

// Choose ID of test file to transmit: 1, 2, 3, 4, 5 or 6
// Sizes in bytes are: 248, 2085, 6267, 21067, 53228, 141270
std::string file = "2";

// Change to your protocol implementation
framework::IRDTProtocol *createProtocol() {
    return new MyProtocol();
}

// Challenge server address
std::string serverAddress = "networkingchallenges.ewi.utwente.nl";

// Challenge server port
int32_t serverPort = 8002;

// *                                                          *
// **                                                        **
// ***             DO NOT EDIT BELOW THIS LINE!             ***
// ****                                                    ****
// ************************************************************
// ************************************************************
std::string file_timestamp; // only used to write the timestamp for the output file to make finding it easy.

int main(int argc, char *argv[]) {

    if (argc == 2){ // possible file number entered
        std::string temp_file = argv[1];
        if( temp_file.compare("1") == 0 || temp_file.compare("2") == 0 || temp_file.compare("3") == 0
            || temp_file.compare("4") == 0 || temp_file.compare("5") == 0 || temp_file.compare("6") == 0 ){
                file = temp_file;                
        } else {
            printf("Error: The entered argument is not a valid file number, using hardcoded value (%s)!\n", file);
        }
    }

#ifdef _MSC_VER
    // Initialize Winsock
    WSADATA wsaDataUnused;
    WSAStartup(/*version*/2, &wsaDataUnused);
#endif
    std::cout << "[FRAMEWORK] Starting client... " << std::endl;

    // Initialize communication with the simulation server
    framework::DRDTChallengeClient drdtclient(serverAddress, serverPort, groupToken);

    std::cout << "[FRAMEWORK] Done." << std::endl;

    std::cout << "[FRAMEWORK] Press Enter to start the simulation as sender..." << std::endl;
    std::cout
        << "[FRAMEWORK] (Simulation will be started automatically as receiver when the other client in the group issues the start command)"
        << std::endl;

#ifndef _MSC_VER
    // listen for cin non-blocking
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
#endif

    bool startCommand = false;
    while (!drdtclient.isSimulationStarted()
        && !drdtclient.isSimulationFinished()) {

#ifndef _MSC_VER
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        size_t ins = select(1, &read_fds, NULL, NULL, &tv);
        if (!startCommand && ins > 0) {
#else
        // Checks the console for a recent keystroke
        if (_kbhit()) {
#endif
            // Request start as sender.
            drdtclient.requestStart(file);
            startCommand = true;
            while (!drdtclient.isSimulationStarted()
                && !drdtclient.isSimulationFinished()) {
                // Wait until actually started
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }

    std::ostringstream ss;
    ss << time(NULL);
    std::string file_timestamp = ss.str();

    if (drdtclient.isSimulationFinished()) {
        // Finished before actually started indicated failure to start.
        drdtclient.stop();
    }
    else {
        std::cout << "[FRAMEWORK] Simulation started!" << std::endl;

        framework::NetworkLayer networkLayer(&drdtclient);
        // Create a new instance of the protocol
        framework::IRDTProtocol *protocolImpl = createProtocol();
        protocolImpl->setNetworkLayer(networkLayer);
        protocolImpl->setFileID(drdtclient.getFileID());
        if (startCommand) {
            std::cout << "[FRAMEWORK] Running protocol implementation as sender for file " << file << "..." << std::endl;
            std::thread sendThread = std::thread(&framework::IRDTProtocol::sender, protocolImpl);
            drdtclient.getEventLoop()->join(); // wait for the eventloop to finish (due to server signalling finished)
            protocolImpl->setStop(); // signal stop if eventloop finishes.
            sendThread.join(); // wait for sender thread to stop
        }
        else {
            std::cout << "[FRAMEWORK] Running protocol implementation as receiver..." << std::endl;
            std::vector<int32_t> fileContents = protocolImpl->receiver();
            framework::setFileContents(fileContents, file, file_timestamp);
            drdtclient.sendChecksum("OUT", "rdtcOutput" + file + "." + file_timestamp.c_str() + ".png");
        }

        //terminate

        std::cout << "[FRAMEWORK] Shutting down client... " << std::endl;
        delete protocolImpl;
        drdtclient.stop();
        std::cout << "[FRAMEWORK] Done." << std::endl;
    }

#if _MSC_VER
    // De-initialize Winsock
    WSACleanup();
#endif
}
