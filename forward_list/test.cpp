#define BOOST_TEST_MODULE

#include "forward_list.cpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ForwardListTestSuite)

BOOST_AUTO_TEST_CASE(Initialization) {
  forward_list<int> list;
  BOOST_CHECK_EQUAL(list.size(), 0);

  list.push_front(1);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 1);

  list.push_front(2);
  BOOST_CHECK_EQUAL(list.front(), 2);

  list.push_front(3);
  BOOST_CHECK_EQUAL(list.front(), 3);
  BOOST_CHECK_EQUAL(list.size(), 3);
}

BOOST_AUTO_TEST_CASE(CopyConstruction) {

  forward_list<int> l1;
  l1.push_front(1);
  l1.push_front(2);
  l1.push_front(3);

  forward_list<int> l2(l1);

  BOOST_CHECK_EQUAL(l2.front(), 3);
  l2.pop_front();
  BOOST_CHECK_EQUAL(l2.front(), 2);
  l2.pop_front();
  BOOST_CHECK_EQUAL(l2.front(), 1);
}

BOOST_AUTO_TEST_CASE(PushBack) {
  forward_list<int> list;
  list.push_back(1);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 1);

  list.push_back(2);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 2);

  list.push_back(3);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 3);

  list.push_back(4);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 4);

  list.push_back(5);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 5);
}

BOOST_AUTO_TEST_CASE(PushFront) {
  forward_list<int> list;
  list.push_front(1);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 1);

  const auto it = list.begin();
  list.insert_after(it, 2);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 2);

  list.push_front(3);
  BOOST_CHECK_EQUAL(list.front(), 3);
  BOOST_CHECK_EQUAL(list.size(), 3);

  list.push_front(4);
  BOOST_CHECK_EQUAL(list.front(), 4);
  BOOST_CHECK_EQUAL(list.size(), 4);

  list.push_front(5);
  BOOST_CHECK_EQUAL(list.front(), 5);
  BOOST_CHECK_EQUAL(list.size(), 5);
}

BOOST_AUTO_TEST_CASE(PopFront) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  BOOST_CHECK_EQUAL(list.front(), 3);
  list.pop_front();
  BOOST_CHECK_EQUAL(list.front(), 2);
  list.pop_front();
  BOOST_CHECK_EQUAL(list.front(), 1);
  list.pop_front();

  BOOST_CHECK_THROW(list.front(), const char *);
}

BOOST_AUTO_TEST_CASE(Swap) {
  forward_list<int> list1;
  list1.push_front(1);
  list1.push_front(2);

  forward_list<int> list2;
  list2.push_front(3);
  list2.push_front(4);

  swap(list1, list2);

  BOOST_CHECK_EQUAL(list1.front(), 4);
  BOOST_CHECK_EQUAL(list2.front(), 2);
}

BOOST_AUTO_TEST_CASE(AssignmentOperator) {
  forward_list<int> list1;
  list1.push_front(1);
  list1.push_front(2);
  list1.push_front(3);

  forward_list<int> list2;
  list2 = list1;

  BOOST_CHECK_EQUAL(list2.front(), 3);
  list2.pop_front();
  BOOST_CHECK_EQUAL(list2.front(), 2);
  list2.pop_front();
  BOOST_CHECK_EQUAL(list2.front(), 1);
}

BOOST_AUTO_TEST_CASE(Destructor) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  BOOST_CHECK_EQUAL(list.size(), 3);

  list.pop_front();
  BOOST_CHECK_EQUAL(list.size(), 2);

  list.pop_front();
  BOOST_CHECK_EQUAL(list.size(), 1);

  list.pop_front();
  BOOST_CHECK_EQUAL(list.size(), 0);
}

BOOST_AUTO_TEST_CASE(InsertAfter) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  auto it = list.begin();
  ++it; // Move to the second element

  list.insert_after(it, 4);
  BOOST_CHECK_EQUAL(list.front(), 3);
  BOOST_CHECK_EQUAL(list.size(), 4);

  ++it; // Move to the third element
  list.insert_after(it, 5);
  BOOST_CHECK_EQUAL(list.front(), 3);
  BOOST_CHECK_EQUAL(list.size(), 5);
}

BOOST_AUTO_TEST_CASE(Iterator) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(IteratorIncrement) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(IteratorDereference) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(IteratorEquality) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  auto it1 = list.begin();
  auto it2 = list.begin();

  BOOST_CHECK(it1 == it2);
  ++it2;
  BOOST_CHECK(it1 != it2);
}

BOOST_AUTO_TEST_CASE(Reverse) {
  forward_list<int> list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK(it == list.end());

  // Reverse the list
  list.reverse();

  it = list.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(EmptyList) {
  forward_list<int> list;
  BOOST_CHECK(list.empty());

  list.push_front(1);
  BOOST_CHECK(!list.empty());

  list.pop_front();
  BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(ReverseEmptyList) {
  forward_list<int> list;
  BOOST_CHECK(list.empty());
  BOOST_CHECK_THROW(list.front(), const char *);
  BOOST_CHECK_THROW(list.pop_front(), const char *);

  list.reverse();
  BOOST_CHECK(list.empty());
  BOOST_CHECK_THROW(list.front(), const char *);
  BOOST_CHECK_THROW(list.pop_front(), const char *);
}

BOOST_AUTO_TEST_CASE(ReverseSingleElementList) {
  forward_list<int> list;
  list.push_front(1);
  BOOST_CHECK_EQUAL(list.front(), 1);

  list.reverse();
  BOOST_CHECK_EQUAL(list.front(), 1);

  list.push_back(2);
  BOOST_CHECK_EQUAL(list.front(), 1);
  BOOST_CHECK_EQUAL(list.size(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
