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
#include <vector>
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
        int tickCounter = 0;    // For debugging
        std::map<int32_t, my_protocol::MyRoute> myRoutingTable;
<<<<<<< HEAD:Challenge5/my_protocol/MyRoutingProtocol_Soenke.h
        std::map<int32_t, std::vector<my_protocol::MyRoute>> myPossibleRoutes;
=======
        std::vector<int32_t> neighbourTable;
        int timeout = 0;
>>>>>>> 31c4f9025a1c35068511176e5ba7ebe79fd46c97:Challenge5/my_protocol/MyRoutingProtocol.h
    };

} /* namespace my_protocol */

#endif /* MyROUTINGPROTOCOL_H_ */
