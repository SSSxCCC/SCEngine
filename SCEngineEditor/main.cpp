#include "SCEngineEditor.h"

int main() {
    try {
        sc::SCEngineEditor editor;
        editor.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
