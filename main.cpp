#include <iostream>
#include "PelcoDProtocol.hpp"

int main() {
    try {
        std::shared_ptr<SimplePelcoDProtocolImpl> simple = std::make_shared<SimplePelcoDProtocolImpl>();
        std::shared_ptr<SimplePelcoDDecoratorA> decorator1 = std::make_shared<SimplePelcoDDecoratorA>(simple.get());

        decorator1->stopMotion();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}