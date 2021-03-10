/**
 * MyRoutingProtocol.cpp
 *
 *   Version: 2019-03-12
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

#include "MyRoutingProtocol.h"

namespace my_protocol {

    MyRoutingProtocol::MyRoutingProtocol() {
    }

    MyRoutingProtocol::~MyRoutingProtocol() {
    }

    void MyRoutingProtocol::init(framework::LinkLayer *linkLayer) {
        this->linkLayer = linkLayer;
        std::cout << "This is node: " << this->linkLayer->getOwnAddress() << std::endl;
    }

    void MyRoutingProtocol::tick(std::vector<framework::PacketWithLinkCost> packetsWithLinkCosts) {
        if(timeout) {   // Brute force way of allowing new data propagation
            timeout -= 1;
            return;
        }
        // Get the address of this node
        int32_t myAddress = this->linkLayer->getOwnAddress();

        // rint something useful for debugging
        std::cout << "tick:  " << tickCounter << ";"  << packetsWithLinkCosts.size() << " packets\n";
        tickCounter++;

        
        // Keep track of the neighbours to know when one dies
        std::vector<int32_t> currentNeighbours;

        // Process the incoming packets
        for (size_t i = 0; i < packetsWithLinkCosts.size(); i++) {
            framework::Packet packet = packetsWithLinkCosts[i].packet;
            int32_t neighbour = packet.getSourceAddress(); 


            // Keep track of which neighbours are alive
            bool inTable = false;
            for (const auto& entry : neighbourTable) { if(entry == neighbour) inTable = true;}
            if(!inTable) { neighbourTable.push_back(neighbour); }


            // Keep track of the neighbours that send something during this run
            currentNeighbours.push_back(neighbour);



            int32_t linkcost = packetsWithLinkCosts[i].linkCost;   
            framework::DataTable *dt = packet.getDataTable(); 
            std::cout << "received packet from " << neighbour << " with "
                << dt->getNRows() << " rows and "
                << dt->getNColumns() << " columns of data\n";

            //add the neighbours, better routes will be overriden anyway
            int32_t destAddress = neighbour;
            int32_t destCost = 0;

            // Check if the destination address allready exists:
            if (myRoutingTable.count(destAddress)) { 

                // Compare if the existing entry has higher link cost, if not add entry
                if (myRoutingTable[destAddress].cost > linkcost) { 
                    my_protocol::MyRoute r;
                    r.nextHop = neighbour;
                    r.cost = linkcost + destCost;
                    r.Hop1 = 0;
                    r.Hop2 = 0;
                    r.Hop3 = 0;
                    r.Hop4 = 0;
                    myRoutingTable[destAddress] = r;
                }
            }

            // if it does not exist yet just add it
            else { 
                my_protocol::MyRoute r;
                r.nextHop = neighbour;
                r.cost = linkcost + destCost;
                r.Hop1 = 0;
                r.Hop2 = 0;
                r.Hop3 = 0;
                r.Hop4 = 0;
                myRoutingTable[destAddress] = r;
            }



            // Go through the datatable row by row
            for (int j = 0; j < dt->getNRows(); j++) {
                int32_t destAddress = dt->get(j, 0);
                int32_t destCost = dt->get(j, 1);
                int32_t destHop1 = dt->get(j, 2);
                int32_t destHop2 = dt->get(j, 3);
                int32_t destHop3 = dt->get(j, 4);
                int32_t destHop4 = dt->get(j, 5);

                
                // Check if this node is part of the route, if yes ignore it
                if (destAddress == myAddress) continue; 
                if(destHop1 == myAddress) continue;
                if(destHop2 == myAddress) continue;
                if(destHop3 == myAddress) continue;
                if(destHop4 == myAddress) continue;


                if (myRoutingTable.count(destAddress)) { //if the destination address allready exists:
                    if ((destCost > 99999) && (myRoutingTable[destAddress].nextHop == neighbour || myRoutingTable[destAddress].Hop1 == neighbour || myRoutingTable[destAddress].Hop2 == neighbour || myRoutingTable[destAddress].Hop3 == neighbour || myRoutingTable[destAddress].Hop4 == neighbour)){
                        // A node disconnected
                        std::cout << "A node disconnected" << std::endl;
                        myRoutingTable[destAddress].cost = 1000000;
                        myRoutingTable[destAddress].nextHop = 0;
                        myRoutingTable[destAddress].Hop1 = 0;
                        myRoutingTable[destAddress].Hop2 = 0;
                        myRoutingTable[destAddress].Hop3 = 0;
                        myRoutingTable[destAddress].Hop4 = 0;
                        continue;
                    }
                    else if(myRoutingTable[destAddress].nextHop == neighbour && (myRoutingTable[destAddress].cost < destCost + linkcost)){
                        // Link cost increased, Treat it as a broken link
                        for (auto currentNeighbourEntry = currentNeighbours.begin(); currentNeighbourEntry != currentNeighbours.end(); ) {
                            if (*currentNeighbourEntry == neighbour) {
                                currentNeighbourEntry = currentNeighbours.erase(currentNeighbourEntry);
                            } else {
                                ++currentNeighbourEntry;
                            }
                        }
                        timeout = 5;
                    }
                    
                    // Compare if the existing entry has higher link cost, if not add entry
                    else if (myRoutingTable[destAddress].cost > destCost + linkcost) { 
                        std::cout << "Better route: " << myAddress << "->" << destAddress <<". " << myRoutingTable[destAddress].cost << ">" << destCost + linkcost << std::endl;
                        my_protocol::MyRoute r;
                        r.nextHop = neighbour;
                        r.cost = destCost + linkcost;
                        r.Hop1 = destHop1;
                        r.Hop2 = destHop2;
                        r.Hop3 = destHop3;
                        r.Hop4 = destHop4;
                        
                        if(destHop1 == 0){r.Hop1 = neighbour;
                        } else if(destHop2 == 0){r.Hop2 = neighbour;
                        } else if(destHop3 == 0){r.Hop3 = neighbour;
                        } else if(destHop4 == 0){r.Hop4 = neighbour;}
                        myRoutingTable[destAddress] = r;
                    }
                } else { 
                    // If the route does not exist yet just add it
                    my_protocol::MyRoute r;
                    r.nextHop = neighbour;
                    r.cost = linkcost + destCost;
                    r.Hop1 = destHop1;
                    r.Hop2 = destHop2;
                    r.Hop3 = destHop3;
                    r.Hop4 = destHop4;
                    myRoutingTable[destAddress] = r;
                }
            }
        }
        


        // Check if a neighbour dropped out
        if(currentNeighbours.size() < neighbourTable.size()) {
            std::cout << "A neighbour disappeared" << std::endl;
            for (const auto& neighbourEntry : neighbourTable) {
                bool exists = false;
                for (const auto& currentNeighbourEntry : currentNeighbours) {
                    if(currentNeighbourEntry == neighbourEntry) {
                        exists = true;
                    }
                }
                if(!exists) {
                    // We found the missing neighbour
                    // Set the cost to an arbitrary large number (1 mil)
                    for (auto& removeNeighbour : myRoutingTable) {
                        if(removeNeighbour.second.nextHop == neighbourEntry || removeNeighbour.second.Hop1 == neighbourEntry || removeNeighbour.second.Hop2 == neighbourEntry || removeNeighbour.second.Hop3 == neighbourEntry || removeNeighbour.second.Hop4 == neighbourEntry ){
                            removeNeighbour.second.cost = 1000000;
                            removeNeighbour.second.nextHop == 0;
                            removeNeighbour.second.Hop1 == 0;
                            removeNeighbour.second.Hop2 == 0;
                            removeNeighbour.second.Hop3 == 0;
                            removeNeighbour.second.Hop4 == 0;
                        }
                    }
                    
                }
            }
        }


        // Create the datatable with fastest known routes
        framework::DataTable dt = framework::DataTable(6);   
        for (const auto& entry : myRoutingTable) { 
            std::vector<int32_t> addingEntry;
            addingEntry.push_back(entry.first);         //Target
            addingEntry.push_back(entry.second.cost);   //Cost
            addingEntry.push_back(entry.second.Hop1);   //First hop from target
            addingEntry.push_back(entry.second.Hop2);   //Second hop from target
            addingEntry.push_back(entry.second.Hop3);
            addingEntry.push_back(entry.second.Hop4);

            //std::cout << "Entry: [t=" << addingEntry[0] << ", c=" << addingEntry[1] << ", " << addingEntry[2] << ", " << addingEntry[3] << ", " << addingEntry[4] << ", " << addingEntry[5] << "]\n";
            dt.addRow(addingEntry);
        }

        // Send out the packet, with our own address as the source address
        framework::Packet pkt = framework::Packet(myAddress, 0, &dt);
        this->linkLayer->transmit(pkt);
    }

    std::map<int32_t, int32_t> MyRoutingProtocol::getForwardingTable() {
        // This code extracts from your routing table the forwarding table.
        // The result of this method is send to the server to validate and score your protocol.

        //  <Destination, NextHop>
        std::map<int32_t, int32_t> ft;

        // iterate over all entries in the routing table.
        for (std::map<int32_t, MyRoute>::iterator it = myRoutingTable.begin(); it != myRoutingTable.end(); it++) {
            int32_t destination = it->first;

            MyRoute r = it->second;
            int32_t nextHop = r.nextHop;

            ft[destination] = nextHop;
        }

        return ft;
    }

} /* namespace my_protocol */
