"""
Challange 3: Medium Access Control
By 
    Tjeerd Bakker   -   s2097966
    Soenke van Loh  -   s2270862
"""

# Medium Access Challenge, client protocol implementation
#
# Copyright University of Twente, 2018-2019
#
##########################################################################
#                            Copyright notice                            #
#                                                                        #
#             This file may ONLY be distributed UNMODIFIED.              #
# In particular, a correct solution to the challenge must NOT be posted  #
# in public places, to preserve the learning effect for future students. #
##########################################################################

import random

from framework.abstract import AbstractMACProtocol
from framework.transmission_info import TransmissionType


# MEANING OF PRINT MESSAGES:
#     Idle = Client has no data to send
#     Waiting = Client has data to send but is waiting for other clients
#     Sending = Client is sending data in this frame
#     Timeout = Client has data to send but has received a timeout

class MyProtocol(AbstractMACProtocol):
    def __init__(self):
        self.id = random.randint(1, 255)   
        print("Client ID: "+str(self.id))            
        self.frame = -1
        self.waitframes = 0
        self.clients_sending = 0
        self.sending_clients_ids = []
        self.tried_start_sending = False
        self.caused_collision = False
        self.sending = False
        self.in_timeout = False
        
    
    def handle_control_info(self, control_information: int):
        """Keep track of which nodes are currently sending data

        Args:
            control_information (int): Included control information
                -> contains ClientID and QueueLength of that client
        """
        bytearray = control_information.to_bytes(4, 'big')
        clid = bytearray[2]
        clqlen = bytearray[3]
        if clid == self.id:
            return
        
        if clid not in self.sending_clients_ids:
            # New client started sending data
            print(str(self.frame)+": Adding new client: "+str(clid))
            self.sending_clients_ids.append(clid)
            self.clients_sending += 1
            self.waitframes += 1
        if clqlen == 0:
            # Client is done sending data
            print(str(self.frame)+": Removing client: "+str(clid))
            self.sending_clients_ids.remove(clid)
            self.clients_sending -= 1
            # if self.waitframes:
            #     self.waitframes -= 1
        self.clients_sending = len(self.sending_clients_ids)
        return
    


    def get_header(self, queue_length):
        """Create a control_information header integer
        The header is 4 bytes of which 1 is the Client ID and 1 is the Local Queue Length

        Args:
            queue_length (int): Local queue length

        Returns:
            int: control_information header value
        """
        headerlist = [0, 0, self.id, queue_length]
        bytelist = bytes(headerlist)
        control_int = int.from_bytes(bytelist, 'big')
        return control_int
    
    def set_random_timeout(self):
        """
        Set a random timeout to solve multi-collision
        """
        self.waitframes = random.randint(1,3)
        return

    
    def timeslot_available(self, previous_medium_state, control_information, local_queue_length):
        self.frame += 1


        if previous_medium_state != 2:
            # Previous frame was not a collision

            if previous_medium_state != 1:
                # Previous frame was not idle, analyze the control_information
                self.handle_control_info(control_information)
            
            if previous_medium_state == 3 and self.tried_start_sending:
                # Sent a message and did not cause a collision
                self.sending = True
                self.tried_start_sending = False
                self.caused_collision = False

            if self.sending:
                self.caused_collision = False
                

                if self.waitframes:
                    # Let another client speak in this frame
                    print(str(self.frame)+": Waiting")
                    self.waitframes -= 1
                    return TransmissionType.SILENT, 0
                elif not self.waitframes:
                    # It is this client's turn to send data this frame
                    if local_queue_length == 0:
                        # Local queue is empty, send "I'm Done" message
                        print(str(self.frame)+": Sending - STOP message")
                        self.sending = False
                        self.tried_start_sending = False
                        self.caused_collision = False
                        header = self.get_header(0)
                        return TransmissionType.NO_DATA, header
                    else:
                        print(str(self.frame)+": Sending")
                        self.waitframes = self.clients_sending
                        header = self.get_header(local_queue_length)
                        return TransmissionType.DATA, header
                else:
                    print("I SHOULD NOT BE ABLE TO GET HERE")
                    return TransmissionType.SILENT, 0
            
            
            elif local_queue_length > 0 and not self.sending:
                # Send First Message
                if self.in_timeout:
                    if self.waitframes:
                        self.waitframes -= 1
                        print(str(self.frame)+": Timeout - Remaining: "+str(self.waitframes))
                        return TransmissionType.SILENT, 0
                if previous_medium_state == 1:
                    print(str(self.frame)+": Sending - First message")
                elif previous_medium_state == 3:
                    print(str(self.frame)+": Sending - First message - Causing collision")

                self.tried_start_sending = True
                header = self.get_header(local_queue_length)
                return TransmissionType.DATA, header
            else:
                # Client doesn't have any data to send
                print(str(self.frame)+": Idle")
                return TransmissionType.SILENT, 0


        elif previous_medium_state == 2:
            # Collision happened
            if self.tried_start_sending:
                # This client caused the collision by sendint the initial message
                # Send next message in this frame, other clients will wait
                print(str(self.frame)+": Sending - Collision caused by me")
                self.waitframes = 0
                self.tried_start_sending = False
                self.caused_collision = True
                self.sending = True
                header = self.get_header(local_queue_length)
                return TransmissionType.DATA, header
            elif self.sending:
                if self.caused_collision:
                    # Multiple clients (including this one) caused collision simultaneously
                    # Wait a random time before trying again
                    self.sending = False
                    self.tried_start_sending = False
                    self.caused_collision = False
                    self.in_timeout = True
                    self.set_random_timeout()
                    print(str(self.frame)+": Timeout - Multi collission, Remaining: "+str(self.waitframes))
                    return TransmissionType.SILENT, 0
                else:
                    # A new client caused a collision
                    # On the next frame it will be added to the client-list
                    print(str(self.frame)+": Waiting - Encountered collision")
                    return TransmissionType.SILENT, 0
            else:
                if not local_queue_length:
                    # Collision happened but this client didn't want to send anyways
                    print(str(self.frame)+": Idle")
                    return TransmissionType.SILENT, 0
                else:
                    # Collision happened but the client is in timeout
                    print(str(self.frame)+": Timeout, Remaining: "+str(self.waitframes))
                    return TransmissionType.SILENT, 0