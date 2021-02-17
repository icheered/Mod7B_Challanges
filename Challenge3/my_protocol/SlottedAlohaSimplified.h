/**
 * SlottedAlohaSimplified.h
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

#ifndef PROTOCOL_SLOTTEDALOHASIMPLIFIED_H_
#define PROTOCOL_SLOTTEDALOHASIMPLIFIED_H_

namespace my_protocol {

    class SlottedAlohaSimplified : public framework::IMACProtocol {
    public:
        SlottedAlohaSimplified();
        ~SlottedAlohaSimplified();
        TransmissionInfo TimeslotAvailable(MediumState previousMediumState, int32_t controlInformation, int localQueueLength);
    private:
        std::default_random_engine rnd;
    };

} /* namespace my_protocol */

#endif /* PROTOCOL_SLOTTEDALOHASIMPLIFIED_H_ */
