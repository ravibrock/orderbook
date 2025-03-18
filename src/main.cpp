#include <iostream>
#include <string>
#include "server.hpp"
#include "engine.hpp"

int main(int argc, char* argv[]) {
    int port = 8080;
    std::vector<Market> markets;
    std::string usage = "Usage: " + std::string(argv[0]) + " [--port <port>] [--market <ticker> <min> <max>]...";

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
            if (i + 3 < argc) {
                std::string name = argv[++i];
                std::string arg1 = argv[++i];
                std::string arg2 = argv[++i];
                int min, max;
                try {
                    min = std::stoi(arg1);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Not a valid integer: " << arg1 << std::endl;
                    return 1;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Error: Number out of range: " << arg1 << std::endl;
                    return 1;
                }
                try {
                    min = std::stoi(arg1);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Not a valid integer: " << arg2 << std::endl;
                    return 1;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Error: Number out of range: " << arg2 << std::endl;
                    return 1;
                }
                markets.push_back(Market{name, min, max});
            } else {
                std::cerr << "Error: No [ticker, min, max] specified after --market" << std::endl;
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
