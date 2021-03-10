/**
 * MyRoutingProtocol.h
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

#include <map>
#include "MyRoute.h"
#include "../framework/IRoutingProtocol.h"

#ifndef MyROUTINGPROTOCOL_H_
#define MyROUTINGPROTOCOL_H_

namespace my_protocol {

    class MyRoutingProtocol : public framework::IRoutingProtocol {
    public:
        MyRoutingProtocol();
        ~MyRoutingProtocol();
        void init(framework::LinkLayer* linkLayer);
        void tick(std::vector<framework::PacketWithLinkCost> packetsWithLinkCosts);
        std::map<int32_t, int32_t> getForwardingTable();
    private:
        framework::LinkLayer* linkLayer;
        std::vector<int32_t> MyNeighbours;
        // You can use this data structure to store your routing table.
        std::map<int32_t, my_protocol::MyRoute> myRoutingTable;
        std::map<int32_t, std::vector<my_protocol::MyRoute>> myPossibleRoutes;
    };

} /* namespace my_protocol */

#endif /* MyROUTINGPROTOCOL_H_ */