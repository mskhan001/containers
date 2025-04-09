#define BOOST_TEST_MODULE

#include "widget.cpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(WidgetTestSuite)

BOOST_AUTO_TEST_CASE(Initialization)
{
    BOOST_CHECK_NO_THROW(Widget());   // Test default initialization
    BOOST_CHECK_NO_THROW(Widget(42)); // Test initialization with value
}

BOOST_AUTO_TEST_CASE(ThrowCheck)
{
    BOOST_CHECK_THROW(Widget(43), std::invalid_argument); // Should throw
}

BOOST_AUTO_TEST_CASE(CopyConstructor)
{
    Widget w1(42);
    Widget w2(w1); // Copy constructor
    BOOST_CHECK(w1 == w2);
}

BOOST_AUTO_TEST_CASE(MoveConstructor)
{
    Widget w1(42);
    Widget w2(std::move(w1)); // Move constructor
    BOOST_CHECK(w2 == Widget(42));
}

BOOST_AUTO_TEST_CASE(CopyAssignment)
{
    Widget w1(42);
    Widget w2;
    w2 = w1; // Copy assignment
    BOOST_CHECK(w1 == w2);
}

BOOST_AUTO_TEST_CASE(MoveAssignment)
{
    Widget w1(42);
    Widget w2;
    w2 = std::move(w1); // Move assignment
    BOOST_CHECK(w2 == Widget(42));
}

BOOST_AUTO_TEST_CASE(DoSomething)
{
    Widget w1(42);
    BOOST_CHECK_NO_THROW(w1.doSomething()); // Test method call
}

BOOST_AUTO_TEST_CASE(Equality)
{
    Widget w1(42);
    Widget w2(42);
    Widget w3(44);
    BOOST_CHECK(w1 == w2); // Test equality
    BOOST_CHECK(w1 != w3); // Test inequality
}
BOOST_AUTO_TEST_SUITE_END()