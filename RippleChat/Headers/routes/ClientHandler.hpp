#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include <string>

namespace tcp {

    class ClientHandler {
    public:
        ClientHandler(int clientSocket);
        void handle();

    private:
        int clientSocket;
        void processRequest();
    };

} // namespace tcp

#endif // CLIENT_HANDLER_HPP
