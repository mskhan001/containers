#define BOOST_TEST_MODULE CIRCULAR_BUFFER_TEST

#include <boost/test/included/unit_test.hpp>
#include "circular_buffer.cpp"

BOOST_AUTO_TEST_SUITE(CircularBufferTestSuite)

BOOST_AUTO_TEST_CASE(Initialization)
{
    circular_buffer<int, 5> cb;
    BOOST_CHECK_EQUAL(cb.size(), 0);
    BOOST_CHECK_EQUAL(cb.capacity(), 5);
}

BOOST_AUTO_TEST_CASE(Push)
{
    circular_buffer<int, 2> cb;
    cb.push(5);
    BOOST_CHECK_EQUAL(cb.peek(), 5);
    BOOST_CHECK_EQUAL(cb.size(), 1);

    cb.push(10);
    BOOST_CHECK_EQUAL(cb.peek(), 5);
    BOOST_CHECK_EQUAL(cb.size(), 2);

    cb.push(11);
    BOOST_CHECK_EQUAL(cb.peek(), 10);
    BOOST_CHECK_EQUAL(cb.size(), 2);
}
BOOST_AUTO_TEST_SUITE_END()