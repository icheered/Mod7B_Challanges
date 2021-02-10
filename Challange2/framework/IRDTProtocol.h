/**
 * IRDTProtocol.h
 *
 *   Version: 2016-02-11
 *    Author: Jaco ter Braak & Frans van Dijk, University of Twente.
 * Copyright: University of Twente, 2015-2019
 *
 ************************************************************
 *                                                          *
 **                 DO NOT EDIT THIS FILE                  **
 ***         YOU DO NOT HAVE TO LOOK IN THIS FILE         ***
 **** IF YOU HAVE QUESTIONS PLEASE DO ASK A TA FOR HELP  ****
 *****                                                  *****
 ************************************************************
 */

#ifndef IRDTPROTOCOL_H_
#define IRDTPROTOCOL_H_

#include "NetworkLayer.h"
#include "ITimeoutEventHandler.h"

namespace framework {
    class IRDTProtocol : public ITimeoutEventHandler {
    public:
        IRDTProtocol() {
        }
        virtual ~IRDTProtocol() {
        }

        /**
         * Run the protocol as sender. Called from the framework
         */
        virtual void sender() = 0;

        /**
         * Run the protocol as receiver. Called from the framework
         */
        virtual std::vector<int32_t> receiver() = 0;

        /**
         * Set the file ID to use. Called from the framework
         */
        virtual void setFileID(std::string) = 0;

        /**
         * Sets the network layer implementation. This network layer is used for transmitting and receiving packets.
         * @param networkLayer
         */
        virtual void setNetworkLayer(NetworkLayer&) = 0;

        /**
         * Sets the stop boolean. This can be used to signal your sending thread to stop.
         */
        virtual void setStop() = 0;

    private:
        bool stop;
    };
} /* namespace framework */

#endif /* IRDTPROTOCOL_H_ */
