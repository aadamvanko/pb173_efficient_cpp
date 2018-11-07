#include "bfs_visitor.hpp"

void RandomVisitor::edges( int from, EdgeVisitor& yield )
{
    for ( auto t : _succs[ from ] )
        yield.visit(t);
}
