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
        LFS = LFR = LAF = LARcount = LAR = 0;
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
        std::vector<unsigned char> receivedAcks;
        std::ostringstream ss;
        ss << fileContents.size();
        int sentpacks = int(std::stod(ss.str()) / DATASIZE) + 1;
        std::cout << "File length: " << ss.str() << ". Packetsize: "<< DATASIZE << ". Expected nr of sent packages: " << sentpacks << std::endl;


        // keep track of where we are in the data
        uint32_t filePointer = 0;
        std::vector<int32_t> acknowledgement;
        //repaeting serialNumbers

        unsigned char seq = MINseq;
        LAR = seq-1;
        LFS = seq;

        // create a new packet of appropriate size
        // copy databytes from the input file into data part of the packet for every packet in the packet buffer, i.e., after the header
        std::cout << "Before double assignment" << std::endl;
        uint32_t datalen;
        int j = 0;
        while(true) {
            datalen = std::min(DATASIZE, (uint32_t)fileContents.size() - filePointer);
            std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE + datalen);
            for (uint32_t i = 1; i < HEADERSIZE + datalen; i++) {
                pkt[i] = fileContents[filePointer];
                filePointer++;
            }
            pkt[0] = (j % MAXseq);
            packetBuffer.push_back(pkt);
            j++;
            if (filePointer >= fileContents.size()) break;
        }
        std::cout << "The packet buffer holds " << (int)packetBuffer.size() << " Elements." << std::endl;



        while (!stop) {
            // send the packet to the network layer
            //networkLayer->sendPacket(pkt);
            //std::cout << "Sent one packet with header=" << pkt[0] << std::endl;

            /*bool acked = false;
            unsigned int silence = 0;
            while(!acked){    //waiting for aknowledgment
                std::vector<int32_t> packet;
                if (networkLayer->receivePacket(&packet)) {
                    // tell the user
                    std::cout << "Received ack, length=" << packet.size() << "  first byte=" << packet[0] << std::endl;
                    if(packet[0] == seq){
                        acked = true;
                    }
                }
                else {
                    // sleep for ~10ms (or however long the OS makes us wait)
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    silence++;
                }
                if(silence > 70){
                    networkLayer->sendPacket(pkt);
                    std::cout << "Retransmitting one packet with header=" << pkt[0] << std::endl;
                    silence = 0;
                }
            }
            if (seq < MAXseq) seq++; else seq = MINseq;  //increasing seq in circular way
            if(((uint32_t)fileContents.size() - filePointer) == 0){
                done = true;
            }
            */
            if (LFS <= (LARcount + SWS)) {
                networkLayer->sendPacket(packetBuffer[(int32_t)LFS]);
                framework::SetTimeout(1000, this, LFS);
                LFS++;
            }
            if (networkLayer->receivePacket(&acknowledgement)) {
                // tell the user
                std::cout << "Received ack, length=" << acknowledgement.size() << "  first byte=" << acknowledgement[0] << std::endl;
                if ((LAR + 1) % MAXseq == acknowledgement[0]) {
                    LAR = acknowledgement[0];
                    LARcount++;
                }
            }


        }
        // schedule a timer for 1000 ms into the future, just to show how that works:
        //framework::SetTimeout(1000, this, 28);

        // and loop and sleep; you may use this loop to check for incoming acks. 
        // You can control the stop boolean yourself, the framework will set it to true for the sender once the server signals simulation finished.
    }

    std::vector<int32_t> MyProtocol::receiver() {
        std::cout << "Receiving..." << std::endl;

        // create the array that will contain the file contents
        // note: we don't know yet how large the file will be, so the easiest (but not most efficient)
        //   is to reallocate the array every time we find out there's more data
        std::vector<int32_t> fileContents = std::vector<int32_t>(0);

        // loop until we are done receiving the file
        bool stop = false;
        unsigned int silence = 0;
        unsigned char lastseq = MAXseq;
        unsigned char expectseq = MINseq;
        while (!stop) {

            // try to receive a packet from the network layer
            std::vector<int32_t> packet;

            // if we indeed received a packet
            if (networkLayer->receivePacket(&packet)) {

                // tell the user
                std::cout << "Received packet, length=" << packet.size() << "  first byte=" << packet[0] << std::endl;
                if(packet[0] == expectseq){
                // append the packet's data part (excluding the header) to the fileContents array, first making it larger
                    std::cout << "Adding to file... " << std::endl;
                    fileContents.insert(fileContents.end(), packet.begin() + HEADERSIZE, packet.end());
                    lastseq = packet[0];
                    if (expectseq < MAXseq) expectseq++; else expectseq = MINseq;
                }
                std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE);
                pkt[0] = packet[0];
                networkLayer->sendPacket(pkt);
                std::cout << "Sent one ack: " << pkt[0] << std::endl;
                silence = 0;
                // and let's just hope the file is now complete

            }
            else {
                // sleep for ~10ms (or however long the OS makes us wait)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                silence++;
            }
            if(silence > 500){
                std::cout << "Timeout too long, Assume the file was send completely" << std::endl;
                stop = true;
                silence = 0;
            }
        }

        // write to the output file
        return fileContents;
    }

    void MyProtocol::TimeoutElapsed(int32_t tag) {
        if (LARcount < tag) {
            networkLayer->sendPacket(packetBuffer[(int32_t)tag]);
            framework::SetTimeout(1000, this, tag);
        }
    }

    void MyProtocol::setFileID(std::string id) {
        fileID = id;
    }

    void MyProtocol::setNetworkLayer(framework::NetworkLayer &nLayer) {
        networkLayer = &nLayer;
    }

} /* namespace my_protocol */
