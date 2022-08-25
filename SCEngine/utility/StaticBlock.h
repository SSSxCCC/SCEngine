#ifndef _StaticBlock_H_
#define _StaticBlock_H_

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
*/
#define STATIC_BLOCK \
        [[maybe_unused]] static const auto M_CON(_static_block,__LINE__) = []()

#endif // _StaticBlock_H_
