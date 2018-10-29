#include "bfs.hpp"

#include <functional>

template< typename Y >
void Random::edges( int from, Y yield )
{
    for ( auto t : _succs[ from ] )
        yield( t );
}

template void Random::edges<std::function<void(auto)>>(int from, std::function<void(auto)> yield);
