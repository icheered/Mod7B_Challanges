/**
 * DummyProtocol.h
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

#ifndef DUMMYPROTOCOL_H_
#define DUMMYPROTOCOL_H_

#include "../framework/IRDTProtocol.h"
#include "../framework/NetworkLayer.h"
#include "../framework/Utils.h"
#include <cstdio>
#include <iostream>
#include <iterator>
#include <vector>

namespace my_protocol {

    class DummyProtocol : public framework::IRDTProtocol {


    public:
        DummyProtocol();
        ~DummyProtocol();
        void sender();
        std::vector<int32_t> receiver();
        void setNetworkLayer(framework::NetworkLayer&);
        void setFileID(std::string);
        void TimeoutElapsed(int32_t);

    private:
        std::string fileID;
        framework::NetworkLayer* networkLayer;
        bool stop = false;
        const uint32_t HEADERSIZE = 1;   // number of header bytes in each packet
        const uint32_t DATASIZE = 128;   // max. number of user data bytes in each packet
    };

} /* namespace my_protocol */

#endif /* DUMMYPROTOCOL_H_ */
