
#include "target.hpp"
#include "build_graph.hpp"
#include "builder.hpp"

#include <iostream>

int main()
{
	//test 1
	{
		std::cout << "test 1" << std::endl;
		BuildGraph graph( { { 1, 2 },
						  { 2, 3 } } );
		Builder( 1 )
			.Execute( graph, 1 );
	}

	// test 2
	std::cout << "\ntest 2" << std::endl;
	try
	{
		BuildGraph graph( { { 1, 2 },
						  { 2, 3 },
						  { 3, 1 } } ); // cycle
		Builder( 1 )
			.Execute( graph, 1 );
	}
	catch( const std::exception& e )
	{
		std::cout << e.what() << std::endl;
	}

	// test 3
	std::cout << "\ntest 3" << std::endl;
	{
		BuildGraph graph( { { 2, 3 },
						  { 2, 4 },
						  { 4, 5 },
						  { 1, 5 },
						  { 1, 6 },
						  { 6, 8 },
						  { 6, 9 },
						  { 9, 5 },
						  { 9, 8 },
						  { 9, 11 },
						  { 8, 12 },
						  { 5, 10 },
						  { 10, 12 } } );
		Builder( 1 )
			.Execute( graph, 1 );
	}

	// test 4
	std::cout << "\ntest 4" << std::endl;
	try
	{
		BuildGraph graph( { { 2, 3 },
						  { 2, 4 },
						  { 4, 5 },
						  { 1, 5 },
						  { 1, 6 },
						  { 6, 8 },
						  { 6, 9 },
						  { 9, 5 },
						  { 9, 8 },
						  { 9, 11 },
						  { 8, 12 },
						  { 5, 10 },
						  { 10, 12 },
						  { 8, 1 } } ); // cycle
		Builder( 1 )
			.Execute( graph, 1 );
	}
	catch( const std::exception& e )
	{
		std::cout << e.what() << std::endl;
	}

	// test 3
	std::cout << "\ntest 5" << std::endl;
	{
		BuildGraph graph( { { 2, 3 },
						  { 2, 4 },
						  { 4, 5 },
						  { 1, 5 },
						  { 1, 6 },
						  { 6, 8 },
						  { 6, 9 },
						  { 9, 5 },
						  { 9, 8 },
						  { 9, 11 },
						  { 8, 12 },
						  { 5, 10 },
						  { 10, 12 } } );
		Builder( 3 )
			.Execute( graph, 1 );
	}

	return 0;
}
