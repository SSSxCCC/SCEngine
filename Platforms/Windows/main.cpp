#include "SCEngineClient.h"

int main() {
    try {
        sc::SCEngineClient client;
        client.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
