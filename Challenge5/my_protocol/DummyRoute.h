/**
 * DummyRoute.h
 *
 * Simple class which describes a route entry in the forwarding table.
 * Can be extended to include additional data.
 *
 *   Version: 2017-03-16
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

#include <cstdint>

#ifndef DUMMYROUTE_H_
#define DUMMYROUTE_H_
namespace my_protocol {
    class DummyRoute {
    public:
        DummyRoute();
        ~DummyRoute();
        int32_t nextHop = 0;
        int32_t cost = 0;
    };
}
#endif /* DUMMYROUTE_H_ */
