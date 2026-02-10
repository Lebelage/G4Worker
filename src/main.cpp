#include <iostream>

#include "App.h"
int main(int argc, char **argv)
{
    auto app = std::make_unique<G4Worker::App>(argc, argv);
    return 0;
}