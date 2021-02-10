/**
 * MyProtocol.h
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

#ifndef MyProtocol_H_
#define MyProtocol_H_

#include "../framework/IRDTProtocol.h"
#include "../framework/NetworkLayer.h"
#include "../framework/Utils.h"
#include <cstdio>
#include <iostream>
#include <iterator>
#include <vector>

namespace my_protocol {

    class MyProtocol : public framework::IRDTProtocol {


    public:
        MyProtocol();
        ~MyProtocol();
        void sender();
        std::vector<int32_t> receiver();
        void setNetworkLayer(framework::NetworkLayer&);
        void setFileID(std::string);
        void setStop();
        void TimeoutElapsed(int32_t);

    private:
        unsigned char MAXseq = 1;
        unsigned char MINseq = 0;
        std::string fileID;
        framework::NetworkLayer* networkLayer;
        bool stop = false;
        const uint32_t HEADERSIZE = 1;   // number of header bytes in each packet
        const uint32_t DATASIZE = 256;   // max. number of user data bytes in each packet
    };

} /* namespace my_protocol */

#endif /* MyProtocol_H_ */
