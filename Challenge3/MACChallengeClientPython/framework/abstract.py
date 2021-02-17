"""
 *                                                          *
 **                 DO NOT EDIT THIS FILE                  **
 ***         YOU DO NOT HAVE TO LOOK IN THIS FILE         ***
 **** IF YOU HAVE QUESTIONS PLEASE DO ASK A TA FOR HELP  ****
 *****                                                  *****
 ************************************************************
 """

class AbstractMACProtocol:
    def timeslot_available(self, previous_medium_state, control_information, local_queue_length):
        """
        The (emulated) physical layer will announce a new timeslot to the protocol by calling this method.
        :param previous_medium_state: The state of the medium in the last timeslot
        :type previous_medium_state: MediumState
        :param control_information: Control information, if available (only when previous_medium state was SUCCESS).
        :type control_information: int
        :param local_queue_length: The length of the local packet queue.
        :type local_queue_length: int
        :return: A tuple containing the action to perform, and the control information to send.
                 Control information may be ommitted if it is not necessary or if no
                 information is needed to be sent (0 is sent in that case)
        :rtype: tuple(TransmissionType, int) or TransmissionType
        """
        pass
