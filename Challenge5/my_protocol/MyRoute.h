/**
 * MyRoute.h
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

#ifndef MyROUTE_H_
#define MyROUTE_H_
namespace my_protocol {
    class MyRoute {
    public:
        MyRoute();
        ~MyRoute();
        int32_t nextHop = 0;
        int32_t cost = 0;
        int32_t Hop1 = 0;
        int32_t Hop2 = 0;
        int32_t Hop3 = 0;
        int32_t Hop4 = 0;
    };
}
#endif /* MyROUTE_H_ */
