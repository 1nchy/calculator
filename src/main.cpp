#include "frontend.hpp"

int main(void) {
    asp::calculator_frontend::singleton().activate();
    asp::calculator_frontend::singleton().deactivate();
    return 0;
}