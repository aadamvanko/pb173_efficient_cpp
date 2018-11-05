#include <functional>

template void Random::edges<std::function<void(int)>>(int from, std::function<void(int)> yield);
