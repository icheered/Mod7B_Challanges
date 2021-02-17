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


class SlottedAlohaSimplified(AbstractMACProtocol):
    def timeslot_available(self, previous_medium_state, control_information, local_queue_length):
        # No data to send, just be quiet
        if local_queue_length == 0:
            print("SLOT - No data to send.")
            return TransmissionType.SILENT, 0

        # Randomly transmit with 60% probability
        if random.randint(0, 100) < 60:
            print("SLOT - Sending data and hope for no collision.")
            return TransmissionType.DATA, 0
        else:
            print("SLOT - Not sending data to give room for others.")
            return TransmissionType.SILENT, 0
