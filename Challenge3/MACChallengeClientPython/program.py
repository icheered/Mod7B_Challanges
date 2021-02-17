# You can change this to import another protocol
from my_protocol.my_protocol import MyProtocol

# Change to your group token (get it from the web interface)
token = "e9cee3cf-4cd7-4b4e-9773-b1dcc1e13971"

# Change to your protocol implementation (don't forget to import it first!)
multiple_access_protocol = MyProtocol

# Challenge server address
server_address = "networkingchallenges.ewi.utwente.nl"

# Challenge server port
server_port = 8003

###
# DO NOT EDIT BELOW THIS LINE!
###
import sys
from framework.framework import Program

if __name__ == "__main__":
    program = Program(token=token, protocol=multiple_access_protocol, server=server_address, port=server_port)
    if program.token == "get-token-from-web-interface":
        print("Please enter your token in the file 'program.py'. You can get a token from the web interface.")
        sys.exit(1)

    program.start_framework()
