void init();
void ip2human(unsigned int ip);
int lookup_ip(unsigned int ip);
void add_route(unsigned int p, int prefix_length, int port_number);
void finalize_routes();
