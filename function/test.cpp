#define BOOST_TEST_MODULE

#include "my_function.cpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(my_function_test)

BOOST_AUTO_TEST_CASE(Initialization)
{
    my_function<int(int)> f([](int x)
                            { return x * 2; });
    BOOST_CHECK(f(5) == 10);
}

BOOST_AUTO_TEST_CASE(CopyConstructor)
{
    my_function<int(int)> f([](int x)
                            { return x * 2; });
    my_function<int(int)> f_copy(f);
    BOOST_CHECK(f_copy(5) == 10);
}

BOOST_AUTO_TEST_CASE(CopyAssignment)
{
    my_function<int(int)> f([](int x)
                            { return x * 2; });
    my_function<int(int)> f_copy;
    f_copy = f;
    BOOST_CHECK(f_copy(5) == 10);
}

BOOST_AUTO_TEST_CASE(MoveConstructor)
{
    my_function<int(int)> f([](int x)
                            { return x * 2; });
    my_function<int(int)> f_move(std::move(f));
    BOOST_CHECK(f_move(5) == 10);
}

BOOST_AUTO_TEST_CASE(MoveAssignment)
{
    my_function<int(int)> f([](int x)
                            { return x * 2; });
    my_function<int(int)> f_move;
    f_move = std::move(f);
    BOOST_CHECK(f_move(5) == 10);
}

BOOST_AUTO_TEST_CASE(Callable)
{
    my_function<int(int)> f([](int x)
                            { return x * 2; });
    BOOST_CHECK(f(5) == 10);
}

BOOST_AUTO_TEST_CASE(EmptyFunction)
{
    my_function<int(int)> f;
    BOOST_CHECK_THROW(f(5), bad_function_call);
}

BOOST_AUTO_TEST_SUITE_END()