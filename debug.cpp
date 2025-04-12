/*
helper macros for debugging
*/
#define DEB()                                     \
    {                                             \
        std::cout << __PRETTY_FUNCTION__ << "\n"; \
    }
#define DEBN(x)                                                      \
    {                                                                \
        std::cout << __PRETTY_FUNCTION__ << " " #x "=" << x << "\n"; \
    }