#include <SFML/Network.hpp>
#include <iostream>
#include "World.h"

int main() {
    World world;
    sf::Clock clock;
    clock.restart();
    while (world.IsRunning()) {
        world.Update(clock.getElapsedTime());
    }
    return 0;
}