/**
 * MyProtocol.cpp
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

#include "MyProtocol.h"

namespace my_protocol {



    MyProtocol::MyProtocol() {
        unsigned int seed = std::chrono::steady_clock::now().time_since_epoch().count(); // This grabs the lower 32 bits for our seed.
        this->rnd = std::default_random_engine(seed);
		cnt = 0;
		MyserialNumber = this->rnd() % 1000000;
		std::cout << MyserialNumber << std::endl;
		collisionHappend = sendSerial = trySending = false;
		waitFor = 0;
    }

    MyProtocol::~MyProtocol() {
    }

    TransmissionInfo MyProtocol::TimeslotAvailable(
        MediumState previousMediumState, int32_t controlInformation, int localQueueLength) {
        //check if there is a new user that needs to be added
		if (controlInformation > 0 && controlInformation < queMultiplyer) {
			user tempUser;
			tempUser.serialNumber = controlInformation;
			userList.emplace_back(tempUser);
		}

		//send serialNumber out untill it got recognized
		if (!sendSerial) {
			if (previousMediumState == Success && trySending) {
				sendSerial = true;
				std::cout << "send serial" << std::endl;
				return TransmissionInfo{ Silent,0 };
			}
			else {
				trySending = false;
				if (this->rnd() % 100 < 60) {
					std::cout << "SLOT - tryig to send serial." << std::endl;
					trySending = true;
					return TransmissionInfo{ NoData, MyserialNumber };
				}
				else
					std::cout << "SLOT - Not sending data to give room for others." << std::endl;
					return TransmissionInfo{ Silent, 0};
			}
		}

		//beginning of the acutall data transfer
		else {
			if (previousMediumState == Collision) collisionHappend = true;
			if (previousMediumState == Success) {
				if (collisionHappend && !trySending && waitFor > 0) waitFor--;
				collisionHappend = false;
			}
			trySending = false;
			if (!waitFor) {
				for (auto& users : userList) {
					if (users.que) waitFor++;
					if (users.serialNumber == MyserialNumber) {
						if (localQueueLength > 0) {
							users.que = true;
						}
						else users.que = false;
					}
				}
				if (localQueueLength == 0) { //if I have no data Im not gonna do anything
					std::cout << "SLOT - No data to send." << std::endl;
					return TransmissionInfo{ Silent, 0 };
				}
				else {
					if (collisionHappend) {
						if (this->rnd() % 100 < 25) {
							std::cout << "SLOT - tryig to send Data." << std::endl;
							return TransmissionInfo{ Data, 0 };
							trySending = true;
						}
						else {
							std::cout << "SLOT - Could not send Data." << std::endl;
							//waitFor--;
							return TransmissionInfo{ Silent, 0 };
						}
					}
					else {
						std::cout << "SLOT - Sending data" << std::endl;
						return TransmissionInfo{ Data, 0 };
					}
				}
			}
			else {
				std::cout << "SLOT - Not sending data to give room for others." << std::endl;
				waitFor--;
				return TransmissionInfo{ Silent, 0 };
			}
		}

    }

	

} /* namespace my_protocol */
