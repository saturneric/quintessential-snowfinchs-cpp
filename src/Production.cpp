#include "Production.h"

Production::Production(int index, int left, std::vector<int> right)
    : index(index), left(left), right(std::move(right)) {}
