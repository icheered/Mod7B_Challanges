/**
 * main.cpp
 *
 *   Version: 2019-03-03
 *    Author: Jaco ter Braak & Frans van Dijk, University of Twente.
 * Copyright: University of Twente, 2015-2019
 */

#include <iostream>
#include <string>
#include <sys/types.h>
#include <cstdint>

#ifdef _MSC_VER
#include <conio.h>
#else
#include <sys/select.h>
#endif

#include "../framework/MACChallengeClient.h"
#include "MyProtocol.h"

using namespace my_protocol;

// Change to your group authentication token
std::string groupToken = "get-your-token-from-the-website";

// The protocol implementation, change below
framework::IMACProtocol *protocolImpl = new MyProtocol();

// Challenge server address
std::string serverAddress = "networkingchallenges.ewi.utwente.nl";

// Challenge server port
int serverPort = 8003;

// *                                                          *
// **                                                        **
// ***             DO NOT EDIT BELOW THIS LINE!             ***
// **** IF YOU HAVE QUESTIONS PLEASE DO ASK A TA FOR HELP  ****
// *****                                                  *****
// ************************************************************
// ************************************************************
int main() {
#ifdef _MSC_VER
    // Initialize Winsock
    WSADATA wsaDataUnused;
    WSAStartup(/*version*/2, &wsaDataUnused);
#endif

    std::cout << "[FRAMEWORK] Starting client... " << std::endl;

    framework::MACChallengeClient macclient(serverAddress, serverPort, groupToken);

    macclient.setListener(protocolImpl);

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
    while (!macclient.isSimulationStarted()
        && !macclient.isSimulationFinished()) {

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
            macclient.requestStart();
            startCommand = true;
        }
    }

    std::cout << "[FRAMEWORK] Simulation started!" << std::endl;

    // Wait until the simulation ends
    while (!macclient.isSimulationFinished()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[FRAMEWORK] Simulation stopped! Check your performance on the server web interface." << std::endl;

    std::cout << "[FRAMEWORK] Shutting down client... " << std::endl;
    macclient.stop();
    delete protocolImpl;
    std::cout << "[FRAMEWORK] Done." << std::endl;

#if _MSC_VER
    // De-initialize Winsock
    WSACleanup();
#endif
}
