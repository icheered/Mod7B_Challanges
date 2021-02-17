/**
 * MyProtocol.h
 *
 *   Version: 2015-02-15
 *    Author: Jaco ter Braak, University of Twente.
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

#include "../framework/IMACProtocol.h"
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <random>
#include <iostream>
#include <vector> 
#include <string>

#ifndef PROTOCOL_MYPROTOCOL_H_
#define PROTOCOL_MYPROTOCOL_H_

namespace my_protocol {

    class MyProtocol : public framework::IMACProtocol {
    public:
        MyProtocol();
        ~MyProtocol();
        TransmissionInfo TimeslotAvailable(MediumState previousMediumState, int32_t controlInformation, int localQueueLength);
    private:
        std::default_random_engine rnd;
		int cnt, waitFor;
		int32_t MyserialNumber;
		bool sendSerial, trySending, collisionHappend;
		const int32_t queMultiplyer= pow(2, 20);
		struct user {
			int32_t serialNumber = 0;
			bool que = false;
		};
		std::vector<user> userList;
    };

} /* namespace my_protocol */

#endif /* PROTOCOL_MYPROTOCOL_H_ */
