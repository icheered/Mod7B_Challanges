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
    }

    void MyRoutingProtocol::tick(std::vector<framework::PacketWithLinkCost> packetsWithLinkCosts) {
        // Get the address of this node
        int32_t myAddress = this->linkLayer->getOwnAddress();
        std::cout << "This is Node " << myAddress << std::endl;


        // print something useful for debugging
        std::cout << "tick; received " << packetsWithLinkCosts.size() << " packets\n";
        std::vector<int32_t> MyCurrentNeighbours;

        size_t i;
        // first process the incoming packets; loop over them:
        for (i = 0; i < packetsWithLinkCosts.size(); i++) {
            framework::Packet packet = packetsWithLinkCosts[i].packet; //fetch current package
            int32_t neighbour = packet.getSourceAddress();         // from whom is the packet?

            MyCurrentNeighbours.push_back(neighbour); //only saves my currently existing neighbours

            //save neighbour in neighbour list
            bool exists = false;
            for (const auto& entry : MyNeighbours) {
                if (entry == neighbour) exists = true;
            }
            if (!exists) {
            MyNeighbours.push_back(neighbour); 
            }

            int32_t linkcost = packetsWithLinkCosts[i].linkCost;   // what's the link cost from/to this neighbour?
            framework::DataTable *dt = packet.getDataTable();      // other data contained in the packet
            std::cout << "received packet from " << neighbour << " with "
                << dt->getNRows() << " rows and "
                << dt->getNColumns() << " columns of data\n";



            int32_t destAddress = neighbour;
            int32_t destCost = 0;



            //check if existing paths have changed
            for (int j = 0; j < dt->getNRows(); j++) {
                int32_t nextHop = dt->get(j, 2);
                destAddress = dt->get(j, 0);
                destCost = dt->get(j, 1);
                if (myRoutingTable.count(destAddress)) { //if the destination address allready exists:
                    if (myRoutingTable[destAddress].nextHop == nextHop) {
                        myRoutingTable[destAddress].cost = destCost + linkcost;
                    }
                }
            }

            //add the neighbours if necessary (mostly used for introduction purposes)
            if (myRoutingTable.count(destAddress)) { //if the destination address allready exists:
                if (myRoutingTable[destAddress].cost > linkcost) { //compare if the existing entry has higher link cost, if not add entry
                    my_protocol::MyRoute r;
                    r.nextHop = neighbour;
                    r.cost = linkcost + destCost;
                    myRoutingTable[destAddress] = r;
                }
            }
            else { // if it does not exist yet just add it
                my_protocol::MyRoute r;
                r.nextHop = neighbour;
                r.cost = linkcost + destCost;
                myRoutingTable[destAddress] = r;
            }


            // go through the datatable row by row and compare the entries for shortest path length
            for (int j = 0; j < dt->getNRows(); j++) {
                int32_t nextHop = dt->get(j, 2);
                destAddress = dt->get(j, 0);
                destCost = dt->get(j, 1);
                if (destAddress == myAddress) continue; //check if myAddress is the source of the entry, if yes ignore it
                if (myRoutingTable.count(destAddress)) { //if the destination address allready exists:
                    if (myRoutingTable[destAddress].cost > destCost + linkcost || destCost > 1000000) { //compare if the existing entry has higher link cost, if not add entry
                        my_protocol::MyRoute r;
                        r.nextHop = neighbour;
                        r.cost = linkcost + destCost;
                        myRoutingTable[destAddress] = r;
                    }
                }
                else { // if it does not exist yet just add it
                    my_protocol::MyRoute r;
                    r.nextHop = neighbour;
                    r.cost = linkcost + destCost;
                    myRoutingTable[destAddress] = r;
                }
            }


            // you'll probably want to process the data, update your data structures (myRoutingTable) , etc....

            // reading one cell from the DataTable can be done using the  dt->get(row,column)  method

            /* example code for inserting a route into myRoutingTable: works
            my_protocol::MyRoute r;
            r.nextHop = ...someneighbour... ;
            myRoutingTable[...somedestination...]=r;
            */

            /* example code for checking whether some destination is already in myRoutingTable, and accessing it:
            if (myRoutingTable.count( somedest )) {
                // now access the Route as   myRoutingTable[somedest]
            }
            */
        }

        //Did I lose neighbours? 
        if (MyCurrentNeighbours.size() < MyNeighbours.size()) {
            //Which Neighbour Did I lose?
            int32_t lostNeighbour {0};
            bool found = false;
            for (const auto& n : MyNeighbours) {
                for (const auto& cn : MyCurrentNeighbours) {
                    if (n == cn) found = true;
                }
                if (!found) {
                    lostNeighbour = n;
                    break;
                }
                else {
                    found = false;
                }
            }
            // I know lostneighbour
            for (auto& entry : myRoutingTable) { //I assign "infinite" cost to this neighbour
                if (entry.second.nextHop == lostNeighbour) {
                    entry.second.cost = 1000001;
                }
            }
            //I have to delete My lostNeighbour now
            MyCurrentNeighbours.clear();
        }

        // then send out one (or more, if you want) distance vector packets
        // the actual distance vector data must be stored in the DataTable structure
        framework::DataTable dt = framework::DataTable(3);   // the 3 is the number of columns, you can change this
        for (const auto& entry : myRoutingTable) { //add routing table to dataTable
            //implement the neighbour check here:
                std::vector<int32_t> addingEntry;
                addingEntry.reserve(2);
                addingEntry.emplace_back(entry.first);
                addingEntry.emplace_back(entry.second.cost);
                addingEntry.emplace_back(entry.second.nextHop);
                dt.addRow(addingEntry);
        }
        
        //Data in Datatable will be stored: destAddress, cost (, nextHop)

        // you'll probably want to put some useful information into dt here
        // by using the  dt.set(row, column, value)  method.

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
