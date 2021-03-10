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
        // Get the address of this node
        int32_t myAddress = this->linkLayer->getOwnAddress();

        // print something useful for debugging
        std::cout << "tick:  " << tickCounter << ";"  << packetsWithLinkCosts.size() << " packets\n";
        tickCounter++;

        // Process the incoming packets
        // Keep track of the neighbours to know when one dies
        std::vector<int32_t> currentNeighbours;
        size_t i;
        for (i = 0; i < packetsWithLinkCosts.size(); i++) {
            framework::Packet packet = packetsWithLinkCosts[i].packet;
            int32_t neighbour = packet.getSourceAddress();         // from whom is the packet?

            // Keep track of which neighbours are alive initially
            bool inTable = false;
            for (const auto& entry : neighbourTable) { //add routing table to dataTable
                if(entry == neighbour) inTable = true;
            }
            if(!inTable) {
                std::cout << "New Neighbour: " << neighbour << std::endl;
                neighbourTable.push_back(neighbour);
            }


            // Keep track of the neighbours that send something during this run
            currentNeighbours.push_back(neighbour);




            int32_t linkcost = packetsWithLinkCosts[i].linkCost;   // what's the link cost from/to this neighbour?
            framework::DataTable *dt = packet.getDataTable();      // other data contained in the packet
            std::cout << "received packet from " << neighbour << " with "
                << dt->getNRows() << " rows and "
                << dt->getNColumns() << " columns of data\n";

            //add the neighbours, better routes will be overriden anyway
            int32_t destAddress = neighbour;
            int32_t destCost = 0;

            //add the neighbours, better routes will be overriden anyway
            if (myRoutingTable.count(destAddress)) { //if the destination address allready exists:
                if (myRoutingTable[destAddress].cost > linkcost) { //compare if the existing entry has higher link cost, if not add entry
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
            else { // if it does not exist yet just add it
                my_protocol::MyRoute r;
                r.nextHop = neighbour;
                r.cost = linkcost + destCost;
                r.Hop1 = 0;
                r.Hop2 = 0;
                r.Hop3 = 0;
                r.Hop4 = 0;
                myRoutingTable[destAddress] = r;
            }



            // go through the datatable row by row
            for (int j = 0; j < dt->getNRows(); j++) {
                int32_t destAddress = dt->get(j, 0);
                int32_t destCost = dt->get(j, 1);
                int32_t destHop1 = dt->get(j, 2);
                int32_t destHop2 = dt->get(j, 3);
                int32_t destHop3 = dt->get(j, 4);
                int32_t destHop4 = dt->get(j, 5);

                
                if (destAddress == myAddress) continue; //check if myAddress is entry of data table, if yes ignore it
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
                    else if (myRoutingTable[destAddress].cost > destCost + linkcost) { //compare if the existing entry has higher link cost, if not add entry
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
                        //std::cout << "Nexthop: " << myRoutingTable[destAddress].nextHop << ", Cost: " << myRoutingTable[destAddress].cost << std::endl;
                    }
                } else { // if it does not exist yet just add it
                    if (destCost > 999999) {
                        std::cout << "Received a disconnected node path" << std::endl;
                        continue;
                    }
                    //std::cout << "New route: " << myAddress << "->" << destAddress << ": " << destCost + linkcost << std::endl;
                    my_protocol::MyRoute r;
                    r.nextHop = neighbour;
                    r.cost = linkcost + destCost;
                    r.Hop1 = neighbour;
                    r.Hop2 = 0;
                    r.Hop3 = 0;
                    r.Hop4 = 0;
                    myRoutingTable[destAddress] = r;
                }
            }
        }
        


        
        std::cout << "Current N="<<currentNeighbours.size() << ", [";
        for (const auto& entry : currentNeighbours) {
            std::cout << entry << ", ";
        }
        std::cout << "]\n";

        std::cout << "Known N="<<neighbourTable.size() << ", [";
        for (const auto& entry : neighbourTable) {
            std::cout << entry << ", ";
        }
        std::cout << "]\n";

        // Check if a neighbour dropped out
        if(currentNeighbours.size() < neighbourTable.size()) {
            std::cout << "A neighbour disappeared" << std::endl;
            // This operation is very inefficient if there is a large number of neighbours but since in our scenario there are only 6 nodes it should be fine
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
                        //std::cout << "Nexthop" << removeNeighbour.second.nextHop << std::endl;
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

        // then send out one (or more, if you want) distance vector packets
        // the actual distance vector data must be stored in the DataTable structure
        framework::DataTable dt = framework::DataTable(6);   // the 3 is the number of columns, you can change this
        for (const auto& entry : myRoutingTable) { //add routing table to dataTable
            std::vector<int32_t> addingEntry;
            addingEntry.push_back(entry.first);
            addingEntry.push_back(entry.second.cost);
            addingEntry.push_back(entry.second.Hop1);
            addingEntry.push_back(entry.second.Hop2);
            addingEntry.push_back(entry.second.Hop3);
            addingEntry.push_back(entry.second.Hop4);

            std::cout << "Entry: [t=" << addingEntry[0] << ", c=" << addingEntry[1] << ", " << addingEntry[2] << ", " << addingEntry[3] << ", " << addingEntry[4] << ", " << addingEntry[5] << "]\n";

            //std::cout << "[" << addingEntry[0] << ", " << addingEntry[1] << "]" << std::endl;
            dt.addRow(addingEntry);
        }




        // next, actually send out the packet, with our own address as the source address
        // and 0 as the destination address: that's a broadcast to be received by all neighbours.
        framework::Packet pkt = framework::Packet(myAddress, 0, &dt);
        this->linkLayer->transmit(pkt);

        /*
         Instead of using Packet with a DataTable you may also use Packet with a std::vector<unsigned char>
         as data part, if you really want to send your own data structure yourself.
         Read the documentation in Packet.h to see how you can do this.
         PLEASE NOTE! Although we provide this option we do not support it.
         */
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
