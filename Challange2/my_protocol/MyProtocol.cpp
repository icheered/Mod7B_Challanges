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
        int sentpacks = int(std::stod(ss.str()) / DATASIZE) + 1;
        std::cout << "File length: " << ss.str() << ". Packetsize: "<< DATASIZE << ". Expected nr of sent packages: " << sentpacks << std::endl;


        // uint32_t filePointer = 0;
        // bool done_inserting = false;
        // uint32_t datalen;
        // while(!done_inserting){
        //     datalen = std::min(DATASIZE, (uint32_t)fileContents.size() - filePointer);
        //     std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE + datalen);
        //     //append to packet buffer
        //     if(((uint32_t)fileContents.size() - filePointer) == 0) {
        //         break;
        //     }
        // }


        // keep track of where we are in the data
        uint32_t filePointer = 0;

        //repeating serialNumbers
        unsigned char seq = MINseq;

        //buffer for sws

        bool done = false;
        while(!done){
            // create a new packet of appropriate size
            uint32_t datalen = std::min(DATASIZE, (uint32_t)fileContents.size() - filePointer);
            std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE + datalen);
            std::vector<int32_t> pktBuffer = std::vector<int32_t>(HEADERSIZE + datalen);
            // write something random into the header byte
            pkt[0] = seq;
            // copy databytes from the input file into data part of the packet, i.e., after the header
            for (uint32_t i = 1; i < HEADERSIZE + datalen; i++) {
                pkt[i] = fileContents[filePointer];
                filePointer++;
            }

            // send the packet to the network layer
            networkLayer->sendPacket(pkt);
            std::cout << "Sent one packet with header=" << pkt[0] << std::endl;

            bool acked = false;
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
            if (seq < MAXseq) seq++; else seq = MINseq;  //reassin seq in circular way
            if(((uint32_t)fileContents.size() - filePointer) == 0){
                done = true;
            }
            
        }
        

        // schedule a timer for 1000 ms into the future, just to show how that works:
        //framework::SetTimeout(1000, this, 28);

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
        //unsigned int silence = 0;

        unsigned char MAXseq = SWS;     // High value of SWS, exclusive
        unsigned char MINseq = 0;       // Low value of SWS, inclusive

        std::vector<int32_t> buffer[SWS]; //Buffer array
        int silence = 0;


        while (!stop) {

            // Try to receive a packet from the network layer
            std::vector<int32_t> packet;
            // If we indeed received a packet
            if (networkLayer->receivePacket(&packet)) {
                silence = 0;
            
                if((packet[0] >= MINseq && packet[0] < MAXseq) or (packet[0] < MAXseq-HEADERSIZE*256)){
                    // Packet fits in the buffer array
                    std::cout << "Received packet: " << packet[0] << std::endl;
                    if(packet[0] < MAXseq-HEADERSIZE*256){
                        buffer[HEADERSIZE*256 - MINseq + packet[0]] = packet; //Insert it at the right location
                    }
                    else{
                        buffer[packet[0]-MINseq] = packet; //Insert it at the right location
                    }
                    // Send ack message
                    std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE);
                    pkt[0] = packet[0];
                    networkLayer->sendPacket(pkt);
                    std::cout << "Sent ack: " << pkt[0] << std::endl;
                }
                else if(packet[0] < MINseq) {
                    // Receiving an already handled package
                    // Resend ack message
                    std::vector<int32_t> pkt = std::vector<int32_t>(HEADERSIZE);
                    pkt[0] = packet[0];
                    networkLayer->sendPacket(pkt);
                    std::cout << "Sent ack: " << pkt[0] << std::endl;
                }

                // Check if the first package in the buffer is the next expected one
                while(buffer[0][0] == MINseq){
                    // Add to output file
                    std::cout << "Adding to file... " << std::endl;
                    packet = buffer[0];
                    fileContents.insert(fileContents.end(), packet.begin() + HEADERSIZE, packet.end());
                    
                    // Shift all elements 1 place left
                    for(int i = 0; i < SWS - 1; i++) {
                        buffer[i] = buffer[i+1];
                    }

                    // Increment MINseq and MAXseq
                    MINseq++;
                    MAXseq++;
                    if(MINseq > 255) {
                        MINseq = 0;
                        MAXseq = SWS;
                    } 
                }
            }
            else {
                // sleep for ~10ms (or however long the OS makes us wait)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                silence++;
            }

            if(silence > 50000){
                stop = true;
            }

            //Terminate how?
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
