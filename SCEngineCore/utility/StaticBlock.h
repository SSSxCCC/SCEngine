#ifndef _StaticBlock_H_
#define _StaticBlock_H_

#include <cstring>

#define M_CON(A, B) M_CON_(A, B)
#define M_CON_(A, B) A##B

/*
A mechanism for executing code statically.

Usage example:
STATIC_BLOCK {
	// static code start
	std::cout << "static_block!" << std::endl;
	// static code end
	return 0;
}();

Note: STATIC_BLOCK use __LINE__ to generate function name to avoid repeat,
however if two STATIC_BLOCK are in the same line of two different source file,
compile will fail due to redefinition. To solve this problem you can use STATIC_BLOCK_(name).
*/
#define STATIC_BLOCK [[maybe_unused]] static const auto M_CON(_static_block_,__LINE__) = []()

/*
A mechanism for executing code statically.

Usage example:
STATIC_BLOCK_(name) { // name can be any string
	// static code start
	std::cout << "static_block!" << std::endl;
	// static code end
	return 0;
}();

STATIC_BLOCK_ is the same as STATIC_BLOCK except that only STATIC_BLOCK_ need a abitrary name as parameter.
The name parameter is to solve redefinition problem of STATIC_BLOCK.
*/
#define STATIC_BLOCK_(name) [[maybe_unused]] static const auto _static_block_##name = []()

#endif // _StaticBlock_H_
