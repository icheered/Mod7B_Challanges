/**
 * main.cpp
 *
 *   Version: 2019-03-12
 *    Author: Jaco ter Braak & Frans van Dijk, University of Twente.
 * Copyright: University of Twente, 2015-2019
 */

#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <conio.h>
#else
#include <sys/select.h>
#endif

#include <sys/types.h>
#include <cstdint>
#include <thread>

#include "../framework/LinkLayer.h"
#include "../framework/IRoutingProtocol.h"
#include "../framework/RoutingChallengeClient.h"
#include "../framework/SimulationState.h"
#include "MyRoutingProtocol.h"


using namespace my_protocol;

// Change to your group authentication token
std::string groupToken = "get-your-token-from-the-website";

// Change to your protocol implementation
framework::IRoutingProtocol *createProtocol() {
    return new MyRoutingProtocol();
}

// Challenge server address
std::string serverAddress = "networkingchallenges.ewi.utwente.nl";

// Challenge server port
int32_t serverPort = 8005;


// *                                                          *
// **                                                        **
// ***             DO NOT EDIT BELOW THIS LINE!             ***
// ****                                                    ****
// ************************************************************
// ************************************************************

int main() {
#ifdef _MSC_VER
    // Initialize Winsock
    WSADATA wsaDataUnused;
    WSAStartup(/*version*/2, &wsaDataUnused);
#endif

    std::cout << "[FRAMEWORK] Starting client... " << std::endl;

    // Initialize communication with the simulation server
    framework::RoutingChallengeClient dvrclient(serverAddress, serverPort, groupToken);
    framework::LinkLayer linkLayer(&dvrclient);

    std::cout << "[FRAMEWORK] Done." << std::endl;

    std::cout << "[FRAMEWORK] Press Enter to start the simulation..." << std::endl;
    std::cout
        << "[FRAMEWORK] (Simulation will also be started automatically if another client in the group issues the start command)"
        << std::endl;

#ifndef _MSC_VER
    // listen for cin non-blocking
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
#endif

    bool startCommand = false;
    while (!dvrclient.IsSimulationRunning()
        && dvrclient.getSimulationState() != framework::Finished) {

#ifndef _MSC_VER
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        size_t ins = select(1, &read_fds, NULL, NULL, &tv);
        if (!startCommand && ins > 0) {
#else
        // Checks the console for a recent keystroke
        if (!startCommand && _kbhit()) {
#endif
            // Request to start simulation
            dvrclient.RequestStart();
            startCommand = true;
        }
    }

    std::cout << "[FRAMEWORK] Simulation started!" << std::endl;

    // Run the client and the protocol
    while (dvrclient.getSimulationState() != framework::Finished) {

        // Wait until we start running a test
        while (dvrclient.getSimulationState() != framework::TestRunning) {
            if (dvrclient.getSimulationState() == framework::Finished)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (dvrclient.getSimulationState() != framework::Finished) {

            std::cout << "[FRAMEWORK] Running test " << std::to_string(dvrclient.getTestID()) << "..." << std::endl;

            // Create a new instance of the protocol
            framework::IRoutingProtocol *protocolImpl = createProtocol();
            // Pass the protocol to the challenge client
            dvrclient.setRoutingProtocolAndTock(protocolImpl);
            // Init the protocol
            protocolImpl->init(&linkLayer);
            // Run the protocol

            while (dvrclient.getSimulationState() == framework::TestRunning) {
                //wait until finished
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            std::cout << "[FRAMEWORK] Test completed." << std::endl;
            delete protocolImpl;
            std::cout << "[FRAMEWORK] Stopped." << std::endl;

            while (dvrclient.getSimulationState() == framework::TestComplete) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    std::cout << "[FRAMEWORK] Simulation finished! Check your performance on the server web interface." << std::endl;

    std::cout << "[FRAMEWORK] Shutting down client... " << std::endl;
    dvrclient.Stop();
    std::cout << "[FRAMEWORK] Done." << std::endl;

#if _MSC_VER
    // De-initialize Winsock
    WSACleanup();
#endif
}
