#include <iostream>
#include <string>
#include "server.hpp"
#include "engine.hpp"

int main(int argc, char* argv[]) {
    int port = 8080;
    std::vector<std::string> markets;
    std::string usage = "Usage: " + std::string(argv[0]) + " [--port <port>] [--market <ticker>]...";

    if (argc == 1) {
        std::cerr << usage << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--port") {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: No port specified after --port" << std::endl;
                std::cerr << usage << std::endl;
                return 1;
            }
        } else if (std::string(argv[i]) == "--market") {
            if (i + 1 < argc) {
                markets.push_back(argv[++i]);
            } else {
                std::cerr << "Error: No market specified after --market" << std::endl;
                std::cerr << usage << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown argument " << argv[i] << std::endl;
            std::cerr << usage << std::endl;
            return 1;
        }
    }

    Server server(port, Engine(markets));
    server.start_server();
    return 0;
}
