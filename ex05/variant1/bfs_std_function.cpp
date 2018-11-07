#include "bfs_std_function.hpp"

void RandomStdFunction::edges(int from, std::function<void(int)> yield)
{
    for ( auto t : _succs[ from ] )
        yield( t );
}
