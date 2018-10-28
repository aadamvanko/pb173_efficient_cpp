#include <set>
#include <list>
#include <vector>
#include <random>
#include <cassert>

struct Random
{
    using State = int;
    using Label = int;

    std::vector< std::vector< int > > _succs;

    Random( int vertices, int edges, unsigned seed = 0 )
    {
        std::mt19937 rand{ seed };
        std::uniform_int_distribution< int > dist( 1, vertices );
        _succs.resize( vertices + 1 );
        std::set< int > connected;
        int last = 1;

        /* first connect everything */
        while ( int( connected.size() ) < vertices ) {
            int next = dist( rand );
            if ( connected.count( next ) )
                continue;
            _succs[ last ].push_back( next );
            connected.insert( next );
            last = next;
            -- edges;
        }

        /* add more edges at random */
        while ( edges > 0 ) {
        next:
            int from = dist( rand );
            int to = dist( rand );
            for ( auto c : _succs[ from ] )
                if ( to == c )
                    goto next;
            _succs[ from ].push_back( to );
            -- edges;
        }
    }

    template< typename Y >
    void edges( int from, Y yield )
    {
        for ( auto t : _succs[ from ] )
            yield( t );
    }

    template< typename Y >
    void initials( Y yield )
    {
        yield( 1 );
    }
};

template< typename G >
std::pair< int, int > bfs( G &graph )
{
    std::list< int > open;
    std::set< int > closed;

    graph.initials( [&]( auto v ) { open.push_back( v ); } );
    int edges = -1, vertices = 0;

    while ( !open.empty() )
    {
        auto v = open.front();
        open.pop_front();

        ++ edges;
        if ( closed.count( v ) )
            continue;

        closed.insert( v );
        ++ vertices;
        graph.edges( v, [&]( auto v ) { open.push_back( v ); } );
    }

    return std::make_pair( vertices, edges );
}

int main()
{
    int vertices = 5000, edges = 80000;
    Random r( vertices, edges );
    auto b = bfs( r );
    assert( b.first == vertices );
    assert( b.second == edges );
    return 0;
}
