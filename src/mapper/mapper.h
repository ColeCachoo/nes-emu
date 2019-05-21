#pragma once

#include "../ram/ram.h"

#include <fstream>
#include <iostream>
#include <stdio.h>

class Mapper
{
public:
    Mapper() = default;
    Mapper(RAM *ram);
    ~Mapper();

private:
    RAM *ram;
};
