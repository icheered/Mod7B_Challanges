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

        // print something useful for debugging
        std::cout << "tick; received " << packetsWithLinkCosts.size() << " packets\n";

        size_t i;
        // first process the incoming packets; loop over them:
        for (i = 0; i < packetsWithLinkCosts.size(); i++) {
            framework::Packet packet = packetsWithLinkCosts[i].packet;
            int32_t neighbour = packet.getSourceAddress();         // from whom is the packet?
            int32_t linkcost = packetsWithLinkCosts[i].linkCost;   // what's the link cost from/to this neighbour?
            framework::DataTable *dt = packet.getDataTable();      // other data contained in the packet
            std::cout << "received packet from " << neighbour << " with "
                << dt->getNRows() << " rows and "
                << dt->getNColumns() << " columns of data\n";

            // you'll probably want to process the data, update your data structures (myRoutingTable) , etc....

            // reading one cell from the DataTable can be done using the  dt->get(row,column)  method

            /* example code for inserting a route into myRoutingTable:
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

        // then send out one (or more, if you want) distance vector packets
        // the actual distance vector data must be stored in the DataTable structure
        framework::DataTable dt = framework::DataTable(3);   // the 3 is the number of columns, you can change this
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
