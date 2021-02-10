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
#include <array>
#include <vector>
#include <list>

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
        LARcount = LAR;
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
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if ((LFS <= (LARcount + SWS)) and (LFS < (int)packetBuffer.size())) {
                networkLayer->sendPacket(packetBuffer[LFS]);
                std::cout << "send packet " << LFS << std::endl;
                framework::SetTimeout(700, this, LFS);
                LFS++;
                
            }
            if (networkLayer->receivePacket(&acknowledgement)) {
                // tell the user
                std::cout << "Received ack: " << acknowledgement[0] << std::endl;
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
        int windowsize = 10;
        std::cout << "Receiving..." << std::endl;

        // create the array that will contain the file contents
        // note: we don't know yet how large the file will be, so the easiest (but not most efficient)
        //   is to reallocate the array every time we find out there's more data
        std::vector<int32_t> fileContents = std::vector<int32_t>(0);

        // loop until we are done receiving the file
        bool stop = false;
        //unsigned int silence = 0;

        int HIGHseq = windowsize;     // High value of SWS, exclusive
        int LOWseq = 0;       // Low value of SWS, inclusive
        std::cout << "HIGHseq: " << windowsize << std::endl;
        std::cout << "Lowseq: " << LOWseq << std::endl;
        

        std::vector<int32_t> buffer[windowsize]; //Buffer array
        std::cout << "Size of buffer: " << sizeof(buffer) << std::endl;

        int silence = 0;


        while (!stop) {
            // Try to receive a packet from the network layer
            //std::cout << "Check3" << std::endl;
            std::vector<int32_t> packet;
            // If we indeed received a packet
            if (networkLayer->receivePacket(&packet)) {
                //std::cout << "Check4" << std::endl;
                silence = 0;
                //std::cout << "LOWseqhere: " << LOWseq << std::endl;
                //std::cout << "HIGHseq: " << HIGHseq << std::endl;
                //std::cout << "packet[0]" << packet[0] << std::endl;

                if(packet[0] >= LOWseq && packet[0] < HIGHseq){
                    std::cout << "Received packet: " << packet[0] << std::endl;
                    //std::cout << "1" << std::endl;
                    if(packet[0] > 10 && packet[0] < (int)(std::max(int(HIGHseq-HEADERSIZE*256), 0))){
                        //std::cout << "2" << std::endl;
                        buffer[HEADERSIZE*256 - LOWseq + packet[0]] = packet; //Insert it at the right location
                        //std::cout << "3" << std::endl;
                    }
                    else{
                        //std::cout << "4" << std::endl;
                        buffer[packet[0]-LOWseq] = packet; //Insert it at the right location
                    }
                    // Send ack message
                    std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE);
                    pkt[0] = packet[0];
                    networkLayer->sendPacket(pkt);
                    std::cout << "Sent ack: " << pkt[0] << std::endl;
                } else if(packet[0] < int(std::max(int(HIGHseq-HEADERSIZE*256), 0))){
                    //std::cout << "Check5" << std::endl;
                    // Packet fits in the buffer array
                    std::cout << "Received packet: " << packet[0] << std::endl;
                    //std::cout << "1" << std::endl;
                    if(packet[0] > 10 && packet[0] < (int)(std::max(int(HIGHseq-HEADERSIZE*256), 0))){
                        //std::cout << "2" << std::endl;
                        buffer[HEADERSIZE*256 - LOWseq + packet[0]] = packet; //Insert it at the right location
                        //std::cout << "3" << std::endl;
                    }
                    else{
                        //std::cout << "4" << std::endl;
                        buffer[packet[0]-LOWseq] = packet; //Insert it at the right location
                    }
                    // Send ack message
                    std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE);
                    pkt[0] = packet[0];
                    networkLayer->sendPacket(pkt);
                    std::cout << "Sent ack: " << pkt[0] << std::endl;
                } else if(packet[0] < LOWseq) {
                    //std::cout << "Check6" << std::endl;
                    std::cout << "Lowseq: " << LOWseq << std::endl;
                    // Receiving an already handled package
                    // Resend ack message
                    std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE);
                    pkt[0] = packet[0];
                    networkLayer->sendPacket(pkt);
                    std::cout << "Sent ack: " << pkt[0] << std::endl;
                }

                //Check if the first package in the buffer is the next expected one
                // std::cout << "Buffer inc" << std::endl;
                // std::cout << "Bufferlen: " << sizeof(buffer) << std::endl;
                // std::cout << "Buffer: " << buffer[0][0] << std::endl;
                // std::cout << "Lowseq: " << LOWseq << std::endl;
                // std::cout << "HIGHseq" << HIGHseq << std::endl;
                while(buffer[0][0] == LOWseq){
                    // Add to output file
                    std::cout << "Adding to file: " << buffer[0][0] << std::endl;
                    packet = buffer[0];
                    fileContents.insert(fileContents.end(), packet.begin() + HEADERSIZE, packet.end());
                    
                    // Shift all elements 1 place left
                    std::cout << "Shifting" << std::endl;
                    for(int i = 0; i < windowsize - 1; i++) {
                        buffer[i] = buffer[i+1];
                    }

                    // Increment MINseq and MAXseq
                    LOWseq++;
                    HIGHseq++;
                    if(LOWseq > 255) {
                        LOWseq = 0;
                        HIGHseq = windowsize;
                    } 
                }
                //std::cout << "Check" << std::endl;
            }
            else {
                // sleep for ~10ms (or however long the OS makes us wait)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                silence++;
            }
            //std::cout << "Check2" << std::endl;
            if(silence > 100){
                stop = true;
            }

            //Terminate how?
        }

        // write to the output file
        return fileContents;
    }

    void MyProtocol::TimeoutElapsed(int32_t tag) {
        if (LARcount < tag) {
            
            std::vector<int32_t> packet = packetBuffer[(int32_t)tag];
            std::cout << "Resending package: " << packet[0] << std::endl;
            //std::cout << "Packet header: " <<  << std::endl;
            networkLayer->sendPacket(packet);
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
