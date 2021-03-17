#include <cstdint>

void connect(void);
int32_t get_received_length(void);
unsigned char* receive(unsigned int timeout);
void send(unsigned char* data, int32_t length);
