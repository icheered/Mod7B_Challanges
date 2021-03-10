/**
 * DummyRoutingProtocol.h
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
#include "DummyRoute.h"
#include "../framework/IRoutingProtocol.h"

#ifndef DUMMYROUTINGPROTOCOL_H_
#define DUMMYROUTINGPROTOCOL_H_

namespace my_protocol {

    class DummyRoutingProtocol : public framework::IRoutingProtocol {
    public:
        DummyRoutingProtocol();
        ~DummyRoutingProtocol();
        void init(framework::LinkLayer* linkLayer);
        void tick(std::vector<framework::PacketWithLinkCost> packetsWithLinkCosts);
        std::map<int32_t, int32_t> getForwardingTable();
    private:
        framework::LinkLayer* linkLayer;

        // You can use this data structure to store your routing table.
        std::map<int32_t, my_protocol::DummyRoute> myRoutingTable;
    };

} /* namespace my_protocol */

#endif /* DUMMYROUTINGPROTOCOL_H_ */
