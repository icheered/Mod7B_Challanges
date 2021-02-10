# Challange 2
### Sender
- SWS: Send Window Size
    - Max number of outstanding (unack'ed) frames
- LAR: Last Ack Received
    - Sequence number of last ack received
- LFS: Last Frame Send
    - Sequence number of last ack received

LFS - LAR <= SWS

### Receiver
- RWS: Receive Window Size
    - Max number of out-of-order frames that the receiver accepts
- LAF: Largest Acceptable Frame
    - ???
- LFR: Last Frame Received
    - Sequence number of the last frame received

LAF - LFR <= RWS




Max timeout = Round Trip Time + max delay
Frame size
    - Larger frame = More likely package drop
