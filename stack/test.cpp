#define BOOST_TEST_MODULE

#include "stack.cpp"
#include "./../Widget/widget.cpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(StackTestSuite)

BOOST_AUTO_TEST_CASE(Initialization)
{
    stack<Widget> st;
    st.push(Widget(1));

    BOOST_CHECK_EQUAL(st.top(), Widget(1));
    BOOST_CHECK_EQUAL(st.count(), 1);
    BOOST_CHECK_EQUAL(st.capacity(), 1);
    BOOST_CHECK_NO_THROW(st.pop());
    BOOST_CHECK_EQUAL(st.count(), 0);
    BOOST_CHECK_EQUAL(st.capacity(), 1);
    BOOST_CHECK_EQUAL(st.empty(), true);
    BOOST_CHECK_THROW(st.pop(), const char *);
}

BOOST_AUTO_TEST_CASE(CopyConstructor)
{
    stack<Widget> st1;
    stack<Widget> st2;

    st1.push(Widget(1));
    st2 = st1; // Copy constructor

    BOOST_CHECK_EQUAL(st1, st2);
}

BOOST_AUTO_TEST_CASE(MoveConstructor)
{
    stack<Widget> st1;
    stack<Widget> st2;

    st1.push(Widget(1));
    st2 = std::move(st1); // Move constructor

    BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(CopyAssignment)
{
    stack<Widget> st1;
    stack<Widget> st2;

    st1.push(Widget(1));
    st2 = st1; // Copy assignment

    BOOST_CHECK(st1 == st2);
}

BOOST_AUTO_TEST_CASE(MoveAssignment)
{
    stack<Widget> st1;
    stack<Widget> st2;

    st1.push(Widget(1));
    st2 = std::move(st1); // Move assignment

    BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(DoSomething)
{
    stack<Widget> st;
    st.push(Widget(1));
    BOOST_CHECK_NO_THROW(st.top().doSomething()); // Test method call
}

BOOST_AUTO_TEST_CASE(Equality)
{
    stack<Widget> st1;
    stack<Widget> st2;

    st1.push(Widget(1));
    st2.push(Widget(1));

    BOOST_CHECK(st1 == st2); // Test equality
    st2.push(Widget(2));
    BOOST_CHECK(st1 != st2); // Test inequality
}

BOOST_AUTO_TEST_CASE(Destructor)
{
    stack<Widget> st;
    st.push(Widget(1));
    BOOST_CHECK_NO_THROW(st.pop()); // Test destructor
}

BOOST_AUTO_TEST_CASE(Empty)
{
    stack<Widget> st;
    BOOST_CHECK_EQUAL(st.empty(), true); // Test empty
    st.push(Widget(1));
    BOOST_CHECK_EQUAL(st.empty(), false);
}

BOOST_AUTO_TEST_CASE(Top)
{
    stack<Widget> st;
    st.push(Widget(1));
    BOOST_CHECK_EQUAL(st.top(), Widget(1)); // Test top
}

BOOST_AUTO_TEST_CASE(Push)
{
    stack<Widget> st;
    st.push(Widget(1));
    BOOST_CHECK_EQUAL(st.count(), 1); // Test push
    BOOST_CHECK_EQUAL(st.capacity(), 1);
}

BOOST_AUTO_TEST_CASE(PushMultiple)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    BOOST_CHECK_EQUAL(st.count(), 2); // Test push multiple
    BOOST_CHECK_EQUAL(st.capacity(), 3);
}

BOOST_AUTO_TEST_CASE(Pop)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 0); // Test pop
    BOOST_CHECK_EQUAL(st.capacity(), 1);
}

BOOST_AUTO_TEST_CASE(Resize)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    BOOST_CHECK_EQUAL(st.count(), 2); // Test resize
    BOOST_CHECK_EQUAL(st.capacity(), 3);
}

BOOST_AUTO_TEST_CASE(Full)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    BOOST_CHECK_EQUAL(st.full(), false); // Test full
    st.push(Widget(3));
    BOOST_CHECK_EQUAL(st.full(), true);
}

BOOST_AUTO_TEST_CASE(TopEmpty)
{
    stack<Widget> st;
    BOOST_CHECK_THROW(st.top(), const char *); // Test top on empty stack
}

BOOST_AUTO_TEST_CASE(PopEmpty)
{
    stack<Widget> st;
    BOOST_CHECK_THROW(st.pop(), const char *); // Test pop on empty stack
}

BOOST_AUTO_TEST_CASE(TopFull)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    BOOST_CHECK_EQUAL(st.top(), Widget(3)); // Test top on full stack
}

BOOST_AUTO_TEST_CASE(PopFull)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 2); // Test pop on full stack
}

BOOST_AUTO_TEST_CASE(PushPop)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    BOOST_CHECK_EQUAL(st.count(), 3);
    BOOST_CHECK_EQUAL(st.top(), Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.top(), 2);
    BOOST_CHECK_EQUAL(st.top(), Widget(2));
    st.pop();
    BOOST_CHECK_EQUAL(st.top(), 1);
    BOOST_CHECK_EQUAL(st.top(), Widget(1));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 0); // Test push and pop
    BOOST_CHECK_THROW(st.top(), const char *);
}

BOOST_AUTO_TEST_CASE(PushPopMultiple)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 2); // Test push and pop multiple
    BOOST_CHECK_EQUAL(st.top(), Widget(2));
}

BOOST_AUTO_TEST_CASE(PushPopResize)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 2); // Test push, pop and resize
    BOOST_CHECK_EQUAL(st.capacity(), 3);
}

BOOST_AUTO_TEST_CASE(PushPopFull)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 2); // Test push, pop and full
    BOOST_CHECK_EQUAL(st.full(), false);
}

BOOST_AUTO_TEST_CASE(PushPopEmpty)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 0); // Test push, pop and empty
    BOOST_CHECK_EQUAL(st.empty(), true);
}

BOOST_AUTO_TEST_CASE(PushPopTop)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.pop();
    BOOST_CHECK_EQUAL(st.top(), Widget(1)); // Test push, pop and top
}

BOOST_AUTO_TEST_CASE(PushPopCount)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 1); // Test push, pop and count
}

BOOST_AUTO_TEST_CASE(PushPopCapacity)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.pop();
    BOOST_CHECK_EQUAL(st.capacity(), 3); // Test push, pop and capacity
}

BOOST_AUTO_TEST_CASE(PushPopFullCount)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.count(), 2); // Test push, pop, full and count
}

BOOST_AUTO_TEST_CASE(PushPopFullCapacity)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    BOOST_CHECK_EQUAL(st.capacity(), 3); // Test push, pop, full and capacity
}

BOOST_AUTO_TEST_CASE(MultiplePushPopResize)
{
    stack<Widget> st;
    st.push(Widget(1));
    st.push(Widget(2));
    st.push(Widget(3));
    st.pop();
    st.push(Widget(4));
    BOOST_CHECK_EQUAL(st.count(), 3); // Test multiple push, pop and resize
    BOOST_CHECK_EQUAL(st.capacity(), 3);
}

BOOST_AUTO_TEST_SUITE_END()