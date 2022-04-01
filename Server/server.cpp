// Chat Server
#include "servernetwork.hpp"

int main(int argc, char *argv[])
{
     ServerNetwork server_network(2525);
     server_network.Run();
     return 0;
}
