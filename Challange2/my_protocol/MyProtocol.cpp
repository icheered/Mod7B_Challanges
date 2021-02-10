/**
 * MyProtocol.cpp
 *
 *   Version: 2016-02-11
 *    Author: Jaco ter Braak & Frans van Dijk, University of Twente.
 * Copyright: University of Twente, 2015-2019
 *
 **************************************************************************
 *                            Copyright notice                            *
 *                                                                        *
 *             This file may ONLY be distributed UNMODIFIED.              *
 * In particular, a correct solution to the challenge must NOT be posted  *
 * in public places, to preserve the learning effect for future students. *
 **************************************************************************
 */

#include "MyProtocol.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace my_protocol {

    MyProtocol::MyProtocol() {
        this->networkLayer = NULL;
    }

    MyProtocol::~MyProtocol() {
    }

    /**
     * This is called by the framework when the server signals the simulation is done.
     * This is mostly usefull for stopping the sender side when the simulation finishes.
     * You may also set stop to true yourself if you want.
     */
    void MyProtocol::setStop(){
        this->stop = true;
    }

    void MyProtocol::sender() {
        std::cout << "Sending..." << std::endl;

        // read from the input file
        std::vector<int32_t> fileContents = framework::getFileContents(fileID);
        std::ostringstream ss;
        ss << fileContents.size();
        std::cout << "File length: " << ss.str() << std::endl;


        // keep track of where we are in the data
        uint32_t filePointer = 0;

        // create a new packet of appropriate size
        uint32_t datalen = std::min(DATASIZE, (uint32_t)fileContents.size() - filePointer);
        std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE + datalen);
        // write something random into the header byte
        pkt[0] = 123;
        // copy databytes from the input file into data part of the packet, i.e., after the header
        for (uint32_t i = 1; i < HEADERSIZE + datalen; i++) {
            pkt[i] = fileContents[filePointer];
            filePointer++;
        }

        // send the packet to the network layer
        networkLayer->sendPacket(pkt);
        std::cout << "Sent one packet with header=" << pkt[0] << std::endl;

        // schedule a timer for 1000 ms into the future, just to show how that works:
        framework::SetTimeout(1000, this, 28);

        // and loop and sleep; you may use this loop to check for incoming acks. 
        // You can control the stop boolean yourself, the framework will set it to true for the sender once the server signals simulation finished.
        while (!stop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::vector<int32_t> MyProtocol::receiver() {
        std::cout << "Receiving..." << std::endl;

        // create the array that will contain the file contents
        // note: we don't know yet how large the file will be, so the easiest (but not most efficient)
        //   is to reallocate the array every time we find out there's more data
        std::vector<int32_t> fileContents = std::vector<int32_t>(0);

        // loop until we are done receiving the file
        bool stop = false;
        while (!stop) {

            // try to receive a packet from the network layer
            std::vector<int32_t> packet;

            // if we indeed received a packet
            if (networkLayer->receivePacket(&packet)) {

                // tell the user
                std::cout << "Received packet, length=" << packet.size() << "  first byte=" << packet[0] << std::endl;

                // append the packet's data part (excluding the header) to the fileContents array, first making it larger
                fileContents.insert(fileContents.end(), packet.begin() + HEADERSIZE, packet.end());

                // and let's just hope the file is now complete
                stop = true;

            }
            else {
                // sleep for ~10ms (or however long the OS makes us wait)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        // write to the output file
        return fileContents;
    }

    void MyProtocol::TimeoutElapsed(int32_t tag) {
        std::cout << "Timer expired with tag=" << tag << std::endl;
    }

    void MyProtocol::setFileID(std::string id) {
        fileID = id;
    }

    void MyProtocol::setNetworkLayer(framework::NetworkLayer &nLayer) {
        networkLayer = &nLayer;
    }

} /* namespace my_protocol */
