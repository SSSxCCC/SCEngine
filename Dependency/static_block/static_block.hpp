#define M_CON(A, B) M_CON_(A, B)
#define M_CON_(A, B) A##B

#define STATIC_BLOCK \
        [[maybe_unused]] static const auto M_CON(_static_block,__LINE__) = []()
