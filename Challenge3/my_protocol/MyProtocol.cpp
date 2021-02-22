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
		MyserialNumber = this->rnd() % 1000000;
		std::cout << MyserialNumber << std::endl;
		transmitting = false;
		cnt = 0;
		collisions = 0;
    }

    MyProtocol::~MyProtocol() {
    }

	TransmissionInfo MyProtocol::TimeslotAvailable(
		MediumState previousMediumState, int32_t controlInformation, int localQueueLength) {
		//std::cout << cnt << ": " << std::endl;
		cnt++;
		switch (previousMediumState) {
		case 1: //there was a collision in the last frame
		{
			collisions++;
				bool inque = false;
				if (que.size()) {
					int beginning = que.front();
					do {
						if (MyserialNumber == que.front())
							inque = true;
						que.push(que.front());
						que.pop();
					} while (que.front() != beginning);
				}
				if (!inque && localQueueLength) {
					//std::cout << "coll1" << std::endl;
					if (collisions < 2) {
						//std::cout << "SLOT - tryig to send Data because I want to get into the queue " << MyserialNumber << " turn." << std::endl;
						return TransmissionInfo{ Data, encodecontroleInfo(MyserialNumber, add) };
					}
					else {
						if (this->rnd() % 100 < 60) {
							//std::cout << "SLOT - trying to send data by chance." << std::endl;
							return TransmissionInfo{ Data, add };
						}
						else {
							//std::cout << "SLOT - Not sending data to let new sender into queue (because it was not my lucky day)." << std::endl;
							return TransmissionInfo{ Silent, 0 };
						}
					}
				}
				else {
					//std::cout << "SLOT - Not sending data to let new sender into queue." << std::endl;
					return TransmissionInfo{ Silent, 0 };
				}
			break;
		}
		case 2: //there was successfull transmission in the last frame
		{
			collisions = 0;
			int32_t serial = decodeSerial(controlInformation);
			statusBit status = decodestatusBit(controlInformation);
			//std::cout << "The status bit is " << status << " and the serial number of the sender is: " << serial << std::endl;
			if (status == add) {
				que.push(serial);
				if (serial == MyserialNumber) {
				transmitting = true;
				}
				//std::cout << "pushed " << serial << " in queue" << std::endl;
			}
			else if (status == finish) {
				if (serial == MyserialNumber) {
					transmitting = false;
				}
				//std::cout << "finish 1" << std::endl;
				if (que.front() == serial) {
					que.pop();
					//std::cout << "finish2" << std::endl;
				}
				else {
					//std::cout << "finish3" << std::endl;
					int beginning = que.front();
					do {
						if (que.front() != serial) que.push(que.front());
						que.pop();
					} while (que.front() != beginning);
				}
			}
			if (que.size()) {
				int currentSerial = que.front();
				que.pop();
				que.push(currentSerial);
				//std::cout << "transmitting: " << transmitting << std::endl;
				if (!transmitting && localQueueLength) {
					//std::cout << "SLOT - tryig to send Data because I want to get into the queue " << MyserialNumber << " turn." << std::endl;
					return TransmissionInfo{ Data, encodecontroleInfo(MyserialNumber, add) };
				}
				if (currentSerial == MyserialNumber) {
					if (localQueueLength) {
						//std::cout << "SLOT - tryig to send Data because last transmission was successfull and it is " << MyserialNumber << " turn." << std::endl;
						return TransmissionInfo{ Data, encodecontroleInfo(MyserialNumber, ok) };
					}
					else {
						//std::cout << "SLOT - tryig to send finish because last transmission was successfull and it is " << MyserialNumber << " turn." << std::endl;
						return TransmissionInfo{ NoData, encodecontroleInfo(MyserialNumber, finish) };
					}
				}
				else {
					//std::cout << "SLOT - Not sending data to give room for others." << std::endl;
					return TransmissionInfo{ Silent, 0 };
				}
			}
			else {
				//std::cout << "SLOT - No Data to Send as que is empty." << std::endl;
				return TransmissionInfo{ Silent, 0 };
			}
			break;
		}
		default: //includes case 0 just done this way as I dont exactly know what the initial controlInfo is
		{
			collisions = 0;
			if (localQueueLength) {
				//std::cout << "SLOT - tryig to send Data because channel was idle." << std::endl;
				return TransmissionInfo{ Data, encodecontroleInfo(MyserialNumber, add) };
			}
			else {
				//std::cout << "SLOT - Not sending data as que is empty." << std::endl;
				return TransmissionInfo{ Silent, 0 };
			}
			break;
		}




		}





























	}
	int32_t MyProtocol::encodecontroleInfo(int32_t Serial, statusBit status)  
	{
		return Serial + (int32_t)status*queMultiplyer;
	}
	int32_t MyProtocol::decodeSerial(int32_t controleInfo)
	{
		int temp = controleInfo / queMultiplyer;
		if (temp < 100) return controleInfo;
		else if (temp < 500) return controleInfo - 100*queMultiplyer;
		else return controleInfo - 1000*queMultiplyer;
	}

	MyProtocol::statusBit MyProtocol::decodestatusBit(int32_t controleInfo)
	{
		int temp = controleInfo / queMultiplyer;
		if (temp < 100) return ok;
		else if (temp < 500) return add;
		else return finish;
	}
		//check if there is a new user that needs to be added
		/*if (controlInformation > 0 && controlInformation < queMultiplyer) {
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
		
		
		}*/

	

} /* namespace my_protocol */
