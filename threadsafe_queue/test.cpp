#define BOOST_TEST_MODULE

#include "finegrained_queue.cpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(FineGrainedThreadSafeQueueTestSuite)

BOOST_AUTO_TEST_CASE(Initialization)
{
    threadsafe_queue<int> queue;
    queue.push(1);
    queue.push(2);

    BOOST_CHECK_EQUAL(*queue.try_pop(), 1);
    BOOST_CHECK_EQUAL(*queue.try_pop(), 2);
}

BOOST_AUTO_TEST_SUITE_END()