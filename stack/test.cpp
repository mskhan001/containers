#define BOOST_TEST_MODULE StackTestSuite

#include "./../Widget/widget.cpp" // Assuming Widget definition is here
#include "stack.cpp"              // Assuming your stack class is in stack.cpp
#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp> // Include for specific assertions
#include <vector> // Useful for comparing stack contents easily if needed, or just for structure

// Helper function to create a stack with specific content for testing
template <typename T> stack<T> create_stack_with_data(const std::vector<T> &data) {
  stack<T> s;
  for (const auto &item : data) {
    s.push(item);
  }
  return s;
}

// Define comparison operators for stack<Widget> if not already in stack.cpp
// BOOST_CHECK_EQUAL(st1, st2) requires operator==
// BOOST_CHECK(st1 != st2) requires operator!=
// If these are not in stack.cpp, you might need them here or include a header.
// Assuming operator== and operator!= for stack<T> are correctly implemented
// and operator== for Widget is also implemented.

BOOST_AUTO_TEST_SUITE(StackTestSuite)

// --- Basic State Tests ---

BOOST_AUTO_TEST_CASE(Stack_Empty_Initial) {
  stack<Widget> st;
  BOOST_CHECK(st.empty());
  BOOST_CHECK_EQUAL(st.count(), 0);
  // Initial capacity might be implementation-defined, but should be non-zero or handles it.
  // Based on other tests, it seems to start at 1.
  BOOST_CHECK(st.capacity() >= 0); // Capacity should be non-negative
}

BOOST_AUTO_TEST_CASE(Stack_OperationsOnEmpty_Throw) {
  stack<Widget> st;
  BOOST_CHECK_THROW(st.top(), const char *); // Or specific exception type
  BOOST_CHECK_THROW(st.pop(), const char *); // Or specific exception type
}

// --- Push Operation Tests ---

BOOST_AUTO_TEST_CASE(Stack_PushOneElement) {
  stack<Widget> st;
  Widget w1(10);
  st.push(w1);

  BOOST_CHECK(!st.empty());
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.top(), w1);
  // Initial capacity check (assuming 1 based on existing tests)
  BOOST_CHECK_EQUAL(st.capacity(), 1);
  BOOST_CHECK_EQUAL(st.full(), true); // If capacity is 1 and count is 1
}

BOOST_AUTO_TEST_CASE(Stack_PushMultipleElements_NoResize) {
  // This test is only meaningful if initial capacity > 1.
  // If initial capacity is 1, pushing a second element triggers resize.
  // Let's assume initial capacity is 1 based on existing tests and add tests that
  // specifically target resize behavior.
  // We can skip a 'no resize' multiple push test if capacity is 1 and it immediately resizes.
}

BOOST_AUTO_TEST_CASE(Stack_PushTriggerResize_Once) {
  stack<Widget> st;    // Capacity 1
  st.push(Widget(10)); // Count 1, Capacity 1
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.capacity(), 1);
  BOOST_CHECK_EQUAL(st.top(), Widget(10));

  st.push(Widget(20)); // Triggers resize (to 3 based on existing tests)
  BOOST_CHECK(!st.empty());
  BOOST_CHECK_EQUAL(st.count(), 2);
  // Capacity should have increased
  BOOST_CHECK(st.capacity() > 1);
  BOOST_CHECK_EQUAL(st.capacity(), 3); // Based on existing tests
  BOOST_CHECK_EQUAL(st.top(), Widget(20));
  BOOST_CHECK_EQUAL(st.full(), false); // Count < Capacity
}

BOOST_AUTO_TEST_CASE(Stack_PushToFullAfterResize) {
  stack<Widget> st;    // Capacity 1
  st.push(Widget(10)); // Count 1, Cap 1
  st.push(Widget(20)); // Count 2, Cap 3
  BOOST_CHECK_EQUAL(st.count(), 2);
  BOOST_CHECK_EQUAL(st.capacity(), 3);
  BOOST_CHECK_EQUAL(st.full(), false);

  st.push(Widget(30)); // Count 3, Cap 3 (full)
  BOOST_CHECK(!st.empty());
  BOOST_CHECK_EQUAL(st.count(), 3);
  BOOST_CHECK_EQUAL(st.capacity(), 3);
  BOOST_CHECK_EQUAL(st.top(), Widget(30));
  BOOST_CHECK_EQUAL(st.full(), true);
}

BOOST_AUTO_TEST_CASE(Stack_PushTriggerResize_Multiple) {
  stack<Widget> st;   // Cap 1
  st.push(Widget(1)); // Count 1, Cap 1
  st.push(Widget(2)); // Count 2, Cap 3
  st.push(Widget(3)); // Count 3, Cap 3 (full)
  BOOST_CHECK_EQUAL(st.count(), 3);
  BOOST_CHECK_EQUAL(st.capacity(), 3);
  BOOST_CHECK_EQUAL(st.top(), Widget(3));
  BOOST_CHECK_EQUAL(st.full(), true);

  st.push(Widget(4)); // Triggers another resize (to 6?)
  BOOST_CHECK(!st.empty());
  BOOST_CHECK_EQUAL(st.count(), 4);
  BOOST_CHECK(st.capacity() > 3);  // Capacity should have increased again
  BOOST_CHECK(st.capacity() >= 6); // Assuming a doubling strategy from 3
  BOOST_CHECK_EQUAL(st.top(), Widget(4));
  BOOST_CHECK_EQUAL(st.full(), false);
}

// --- Pop Operation Tests ---

BOOST_AUTO_TEST_CASE(Stack_PopOneElement) {
  stack<Widget> st;
  st.push(Widget(10)); // Count 1, Cap 1
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.capacity(), 1);

  BOOST_CHECK_NO_THROW(st.pop());

  BOOST_CHECK(st.empty());
  BOOST_CHECK_EQUAL(st.count(), 0);
  BOOST_CHECK_EQUAL(st.capacity(), 1); // Capacity usually doesn't decrease on pop
  BOOST_CHECK_EQUAL(st.full(), false);
  BOOST_CHECK_THROW(st.top(), const char *);
  BOOST_CHECK_THROW(st.pop(), const char *);
}

BOOST_AUTO_TEST_CASE(Stack_PopMultipleElements_NoResize) {
  // This is covered by popping from a stack that was resized and then elements removed
}

BOOST_AUTO_TEST_CASE(Stack_PopFromPartialStack) {
  stack<Widget> st;    // Cap 1
  st.push(Widget(10)); // Count 1, Cap 1
  st.push(Widget(20)); // Count 2, Cap 3
  st.push(Widget(30)); // Count 3, Cap 3

  BOOST_CHECK_EQUAL(st.count(), 3);
  BOOST_CHECK_EQUAL(st.capacity(), 3);
  BOOST_CHECK_EQUAL(st.top(), Widget(30));

  BOOST_CHECK_NO_THROW(st.pop()); // Pop 30
  BOOST_CHECK_EQUAL(st.count(), 2);
  BOOST_CHECK_EQUAL(st.capacity(), 3); // Capacity stays the same
  BOOST_CHECK_EQUAL(st.top(), Widget(20));
  BOOST_CHECK_EQUAL(st.full(), false);

  BOOST_CHECK_NO_THROW(st.pop()); // Pop 20
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.capacity(), 3); // Capacity stays the same
  BOOST_CHECK_EQUAL(st.top(), Widget(10));
  BOOST_CHECK_EQUAL(st.full(), false);
}

BOOST_AUTO_TEST_CASE(Stack_PopUntilEmpty_AfterResize) {
  stack<Widget> st;    // Cap 1
  st.push(Widget(10)); // Count 1, Cap 1
  st.push(Widget(20)); // Count 2, Cap 3
  st.push(Widget(30)); // Count 3, Cap 3

  BOOST_CHECK_NO_THROW(st.pop()); // Pop 30
  BOOST_CHECK_NO_THROW(st.pop()); // Pop 20
  BOOST_CHECK_NO_THROW(st.pop()); // Pop 10

  BOOST_CHECK(st.empty());
  BOOST_CHECK_EQUAL(st.count(), 0);
  BOOST_CHECK_EQUAL(st.capacity(), 3); // Capacity usually remains high
  BOOST_CHECK_EQUAL(st.full(), false);
  BOOST_CHECK_THROW(st.top(), const char *);
  BOOST_CHECK_THROW(st.pop(), const char *);
}

// --- Top Operation Tests ---

BOOST_AUTO_TEST_CASE(Stack_TopOnNonEmpty) {
  stack<Widget> st;
  st.push(Widget(10));
  BOOST_CHECK_EQUAL(st.top(), Widget(10));
  st.push(Widget(20));
  BOOST_CHECK_EQUAL(st.top(), Widget(20));
  st.pop();
  BOOST_CHECK_EQUAL(st.top(), Widget(10));
}

BOOST_AUTO_TEST_CASE(Stack_TopMutable) {
  stack<Widget> st;
  st.push(Widget(10));
  BOOST_CHECK_NO_THROW(st.top().doSomething()); // Test mutability via top()
  // Assuming Widget::doSomething changes internal state or observable behavior
  // You might add a check here if doSomething had a side effect you could verify.
}

// --- Combined Push and Pop Sequences ---

BOOST_AUTO_TEST_CASE(Stack_PushPopSequence) {
  stack<Widget> st;
  BOOST_CHECK_EQUAL(st.count(), 0);

  st.push(Widget(1));
  BOOST_CHECK_EQUAL(st.top(), Widget(1));
  BOOST_CHECK_EQUAL(st.count(), 1);
  st.push(Widget(2));
  BOOST_CHECK_EQUAL(st.top(), Widget(2));
  BOOST_CHECK_EQUAL(st.count(), 2);
  st.pop();
  BOOST_CHECK_EQUAL(st.top(), Widget(1));
  BOOST_CHECK_EQUAL(st.count(), 1);
  st.push(Widget(3));
  BOOST_CHECK_EQUAL(st.top(), Widget(3));
  BOOST_CHECK_EQUAL(st.count(), 2);
  st.pop();
  BOOST_CHECK_EQUAL(st.top(), Widget(1));
  BOOST_CHECK_EQUAL(st.count(), 1);
  st.pop();
  BOOST_CHECK_EQUAL(st.count(), 0);
  BOOST_CHECK(st.empty());
  BOOST_CHECK_THROW(st.top(), const char *);
  BOOST_CHECK_THROW(st.pop(), const char *);

  // Test again after refilling
  st.push(Widget(10));
  BOOST_CHECK_EQUAL(st.top(), Widget(10));
  BOOST_CHECK_EQUAL(st.count(), 1);
  st.push(Widget(20));
  BOOST_CHECK_EQUAL(st.top(), Widget(20));
  BOOST_CHECK_EQUAL(st.count(), 2);
  st.push(Widget(30));
  BOOST_CHECK_EQUAL(st.top(), Widget(30));
  BOOST_CHECK_EQUAL(st.count(), 3);
  st.pop();
  BOOST_CHECK_EQUAL(st.top(), Widget(20));
  BOOST_CHECK_EQUAL(st.count(), 2);
}

// --- Copy Constructor Tests ---

BOOST_AUTO_TEST_CASE(Stack_CopyConstructor_Empty) {
  stack<Widget> original;
  stack<Widget> copied = original; // Copy constructor

  BOOST_CHECK(copied.empty());
  BOOST_CHECK_EQUAL(copied.count(), 0);
  // Capacity of copied empty stack might be same as original or default.
  // Hard to check specifically without knowing implementation.
  BOOST_CHECK(original == copied); // operator== check
}

BOOST_AUTO_TEST_CASE(Stack_CopyConstructor_Partial) {
  stack<Widget> original;
  original.push(Widget(10));
  original.push(Widget(20)); // Count 2, Cap 3

  stack<Widget> copied = original; // Copy constructor

  BOOST_CHECK_EQUAL(copied.count(), original.count());
  BOOST_CHECK_EQUAL(copied.capacity(), original.capacity()); // Capacity should be copied
  BOOST_CHECK_EQUAL(copied.top(), original.top());
  BOOST_CHECK(original == copied); // operator== check

  // Test deep copy: Modify original, check copied is unchanged
  original.pop();
  BOOST_CHECK_EQUAL(original.count(), 1);
  BOOST_CHECK_EQUAL(copied.count(), 2); // Copied should retain its state
  BOOST_CHECK_EQUAL(copied.top(), Widget(20));
  BOOST_CHECK(original != copied);
}

BOOST_AUTO_TEST_CASE(Stack_CopyConstructor_Full) {
  stack<Widget> original;   // Cap 1
  original.push(Widget(1)); // Count 1, Cap 1
  original.push(Widget(2)); // Count 2, Cap 3
  original.push(Widget(3)); // Count 3, Cap 3 (full)

  stack<Widget> copied = original; // Copy constructor

  BOOST_CHECK_EQUAL(copied.count(), original.count());
  BOOST_CHECK_EQUAL(copied.capacity(), original.capacity());
  BOOST_CHECK_EQUAL(copied.top(), original.top());
  BOOST_CHECK(original == copied);

  // Test deep copy
  original.pop();
  BOOST_CHECK_EQUAL(original.count(), 2);
  BOOST_CHECK_EQUAL(copied.count(), 3);
  BOOST_CHECK_EQUAL(copied.top(), Widget(3));
  BOOST_CHECK(original != copied);
}

// --- Move Constructor Tests ---

BOOST_AUTO_TEST_CASE(Stack_MoveConstructor_Empty) {
  stack<Widget> original;
  stack<Widget> moved = std::move(original); // Move constructor

  BOOST_CHECK(moved.empty());
  BOOST_CHECK_EQUAL(moved.count(), 0);
  // Capacity of moved-to empty stack likely default
  // BOOST_CHECK_EQUAL(moved.capacity(), expected_default_capacity);

  // Original should be in a valid, but unspecified state (usually empty-like)
  BOOST_CHECK(original.empty());
  BOOST_CHECK_EQUAL(original.count(), 0);
  // Capacity of moved-from stack is typically 0 or default
  BOOST_CHECK(original.capacity() == 0 ||
              original.capacity() == 1); // Assuming initial cap 1 or reset to 0
}

BOOST_AUTO_TEST_CASE(Stack_MoveConstructor_Partial) {
  stack<Widget> original;
  original.push(Widget(10));
  original.push(Widget(20)); // Count 2, Cap 3
  size_t original_capacity = original.capacity();

  stack<Widget> moved = std::move(original); // Move constructor

  BOOST_CHECK(!moved.empty());
  BOOST_CHECK_EQUAL(moved.count(), 2);
  BOOST_CHECK_EQUAL(moved.capacity(), original_capacity); // Capacity should be moved
  BOOST_CHECK_EQUAL(moved.top(), Widget(20));

  // Original should be in a valid, empty-like state
  BOOST_CHECK(original.empty());
  BOOST_CHECK_EQUAL(original.count(), 0);
  BOOST_CHECK(original.capacity() == 0 || original.capacity() == 1); // Moved-from state
}

BOOST_AUTO_TEST_CASE(Stack_MoveConstructor_Full) {
  stack<Widget> original;   // Cap 1
  original.push(Widget(1)); // Count 1, Cap 1
  original.push(Widget(2)); // Count 2, Cap 3
  original.push(Widget(3)); // Count 3, Cap 3 (full)
  size_t original_capacity = original.capacity();

  stack<Widget> moved = std::move(original); // Move constructor

  BOOST_CHECK(!moved.empty());
  BOOST_CHECK_EQUAL(moved.count(), 3);
  BOOST_CHECK_EQUAL(moved.capacity(), original_capacity);
  BOOST_CHECK_EQUAL(moved.top(), Widget(3));
  BOOST_CHECK_EQUAL(moved.full(), true);

  // Original should be in a valid, empty-like state
  BOOST_CHECK(original.empty());
  BOOST_CHECK_EQUAL(original.count(), 0);
  BOOST_CHECK(original.capacity() == 0 || original.capacity() == 1); // Moved-from state
}

// --- Copy Assignment Tests ---

BOOST_AUTO_TEST_CASE(Stack_CopyAssignment_EmptyToEmpty) {
  stack<Widget> st1, st2;
  st1 = st2; // Empty = Empty
  BOOST_CHECK(st1.empty());
  BOOST_CHECK_EQUAL(st1.count(), 0);
  BOOST_CHECK_EQUAL(st1, st2);
}

BOOST_AUTO_TEST_CASE(Stack_CopyAssignment_PartialToEmpty) {
  stack<Widget> st1; // Empty
  stack<Widget> st2; // Partial
  st2.push(Widget(10));
  st2.push(Widget(20)); // Count 2, Cap 3

  st1 = st2; // Empty = Partial

  BOOST_CHECK_EQUAL(st1.count(), 2);
  BOOST_CHECK_EQUAL(st1.top(), Widget(20));
  BOOST_CHECK_EQUAL(st1, st2);

  // Test deep copy
  st2.pop();
  BOOST_CHECK_EQUAL(st2.count(), 1);
  BOOST_CHECK_EQUAL(st1.count(), 2); // st1 should be unchanged
  BOOST_CHECK_EQUAL(st1.top(), Widget(20));
  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_CopyAssignment_FullToEmpty) {
  stack<Widget> st1; // Empty
  stack<Widget> st2; // Full
  st2.push(Widget(1));
  st2.push(Widget(2));
  st2.push(Widget(3)); // Count 3, Cap 3

  st1 = st2; // Empty = Full

  BOOST_CHECK_EQUAL(st1.count(), 3);
  BOOST_CHECK_EQUAL(st1.top(), Widget(3));
  BOOST_CHECK_EQUAL(st1, st2);

  // Test deep copy
  st2.pop();
  BOOST_CHECK_EQUAL(st2.count(), 2);
  BOOST_CHECK_EQUAL(st1.count(), 3); // st1 should be unchanged
  BOOST_CHECK_EQUAL(st1.top(), Widget(3));
  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_CopyAssignment_PartialToPartial) {
  stack<Widget> st1; // Partial (smaller or different)
  st1.push(Widget(99));
  stack<Widget> st2; // Partial
  st2.push(Widget(10));
  st2.push(Widget(20)); // Count 2, Cap 3

  st1 = st2; // Partial = Partial

  BOOST_CHECK_EQUAL(st1.count(), 2);
  BOOST_CHECK_EQUAL(st1.top(), Widget(20));
  BOOST_CHECK_EQUAL(st1, st2);

  // Test deep copy
  st2.pop();
  BOOST_CHECK_EQUAL(st2.count(), 1);
  BOOST_CHECK_EQUAL(st1.count(), 2); // st1 should be unchanged
  BOOST_CHECK_EQUAL(st1.top(), Widget(20));
  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_CopyAssignment_FullToPartial) {
  stack<Widget> st1;    // Partial
  st1.push(Widget(99)); // Count 1, Cap 1
  stack<Widget> st2;    // Full
  st2.push(Widget(1));
  st2.push(Widget(2));
  st2.push(Widget(3)); // Count 3, Cap 3

  st1 = st2; // Full = Partial

  BOOST_CHECK_EQUAL(st1.count(), 3);
  BOOST_CHECK_EQUAL(st1.top(), Widget(3));
  BOOST_CHECK_EQUAL(st1, st2);

  // Test deep copy
  st2.pop();
  BOOST_CHECK_EQUAL(st2.count(), 2);
  BOOST_CHECK_EQUAL(st1.count(), 3); // st1 should be unchanged
  BOOST_CHECK_EQUAL(st1.top(), Widget(3));
  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_CopyAssignment_SelfAssignment) {
  stack<Widget> st;
  st.push(Widget(10));
  st.push(Widget(20));
  st.push(Widget(30)); // Count 3, Cap 3

  stack<Widget> original_state = st; // Save state before self-assignment

  st = st; // Self-assignment

  BOOST_CHECK_EQUAL(st.count(), 3);
  BOOST_CHECK_EQUAL(st.top(), Widget(30));
  BOOST_CHECK_EQUAL(st, original_state); // Should be unchanged
}

// --- Move Assignment Tests ---

BOOST_AUTO_TEST_CASE(Stack_MoveAssignment_EmptyToEmpty) {
  stack<Widget> st1, st2;
  st1 = std::move(st2); // Empty = Empty
  BOOST_CHECK(st1.empty());
  BOOST_CHECK_EQUAL(st1.count(), 0);
  // Check moved-from state
  BOOST_CHECK(st2.empty());
  BOOST_CHECK_EQUAL(st2.count(), 0);
  BOOST_CHECK(st2.capacity() == 0 || st2.capacity() == 1);
}

BOOST_AUTO_TEST_CASE(Stack_MoveAssignment_PartialToEmpty) {
  stack<Widget> st1; // Empty
  stack<Widget> st2; // Partial
  st2.push(Widget(10));
  st2.push(Widget(20)); // Count 2, Cap 3
  size_t st2_capacity_before = st2.capacity();

  st1 = std::move(st2); // Empty = Partial

  BOOST_CHECK(!st1.empty());
  BOOST_CHECK_EQUAL(st1.count(), 2);
  BOOST_CHECK_EQUAL(st1.capacity(), st2_capacity_before); // Capacity moved
  BOOST_CHECK_EQUAL(st1.top(), Widget(20));

  // Check moved-from state
  BOOST_CHECK(st2.empty());
  BOOST_CHECK_EQUAL(st2.count(), 0);
  BOOST_CHECK(st2.capacity() == 0 || st2.capacity() == 1);
}

BOOST_AUTO_TEST_CASE(Stack_MoveAssignment_FullToEmpty) {
  stack<Widget> st1; // Empty
  stack<Widget> st2; // Full
  st2.push(Widget(1));
  st2.push(Widget(2));
  st2.push(Widget(3)); // Count 3, Cap 3
  size_t st2_capacity_before = st2.capacity();

  st1 = std::move(st2); // Empty = Full

  BOOST_CHECK(!st1.empty());
  BOOST_CHECK_EQUAL(st1.count(), 3);
  BOOST_CHECK_EQUAL(st1.capacity(), st2_capacity_before);
  BOOST_CHECK_EQUAL(st1.top(), Widget(3));
  BOOST_CHECK_EQUAL(st1.full(), true);

  // Check moved-from state
  BOOST_CHECK(st2.empty());
  BOOST_CHECK_EQUAL(st2.count(), 0);
  BOOST_CHECK(st2.capacity() == 0 || st2.capacity() == 1);
}

BOOST_AUTO_TEST_CASE(Stack_MoveAssignment_PartialToPartial) {
  stack<Widget> st1; // Partial (will be overwritten)
  st1.push(Widget(99));
  st1.push(Widget(88));
  size_t st1_capacity_before = st1.capacity(); // Likely 3

  stack<Widget> st2; // Partial (source of move)
  st2.push(Widget(10));
  st2.push(Widget(20));                        // Count 2, Cap 3
  size_t st2_capacity_before = st2.capacity(); // Likely 3

  st1 = std::move(st2); // Partial = Partial

  BOOST_CHECK(!st1.empty());
  BOOST_CHECK_EQUAL(st1.count(), 2);
  BOOST_CHECK_EQUAL(st1.capacity(), st2_capacity_before); // st1 now has st2's capacity
  BOOST_CHECK_EQUAL(st1.top(), Widget(20));

  // Check moved-from state
  BOOST_CHECK(st2.empty());
  BOOST_CHECK_EQUAL(st2.count(), 0);
  BOOST_CHECK(st2.capacity() == 0 || st2.capacity() == 1);
}

BOOST_AUTO_TEST_CASE(Stack_MoveAssignment_FullToPartial) {
  stack<Widget> st1;                           // Partial
  st1.push(Widget(99));                        // Count 1, Cap 1
  size_t st1_capacity_before = st1.capacity(); // Likely 1

  stack<Widget> st2; // Full
  st2.push(Widget(1));
  st2.push(Widget(2));
  st2.push(Widget(3));                         // Count 3, Cap 3
  size_t st2_capacity_before = st2.capacity(); // Likely 3

  st1 = std::move(st2); // Full = Partial

  BOOST_CHECK(!st1.empty());
  BOOST_CHECK_EQUAL(st1.count(), 3);
  BOOST_CHECK_EQUAL(st1.capacity(), st2_capacity_before); // st1 now has st2's capacity
  BOOST_CHECK_EQUAL(st1.top(), Widget(3));
  BOOST_CHECK_EQUAL(st1.full(), true);

  // Check moved-from state
  BOOST_CHECK(st2.empty());
  BOOST_CHECK_EQUAL(st2.count(), 0);
  BOOST_CHECK(st2.capacity() == 0 || st2.capacity() == 1);
}

// Self-move assignment (st = std::move(st)) is tricky and often leads to undefined behavior.
// It's usually not required for move assignment to handle this.
// We will *not* add a specific self-move assignment test unless the stack implementation
// is explicitly designed to handle it (which is rare).

// --- Equality and Inequality Tests ---

BOOST_AUTO_TEST_CASE(Stack_Equality_EmptyStacks) {
  stack<Widget> st1, st2;
  BOOST_CHECK(st1 == st2);
}

BOOST_AUTO_TEST_CASE(Stack_Inequality_EmptyVsPartial) {
  stack<Widget> st1; // Empty
  stack<Widget> st2; // Partial
  st2.push(Widget(1));
  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_Equality_IdenticalPartialStacks) {
  stack<Widget> st1;
  st1.push(Widget(10));
  st1.push(Widget(20));

  stack<Widget> st2;
  st2.push(Widget(10));
  st2.push(Widget(20));

  BOOST_CHECK(st1 == st2);
}

BOOST_AUTO_TEST_CASE(Stack_Inequality_DifferentPartialStacks_DifferentTop) {
  stack<Widget> st1;
  st1.push(Widget(10));
  st1.push(Widget(20));

  stack<Widget> st2;
  st2.push(Widget(10));
  st2.push(Widget(30)); // Different top

  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_Inequality_DifferentPartialStacks_DifferentOrder) {
  // Stacks with same elements but pushed in different order should not be equal
  stack<Widget> st1;
  st1.push(Widget(10));
  st1.push(Widget(20));

  stack<Widget> st2;
  st2.push(Widget(20)); // Pushed in different order (this stack is 20 then 10)
  st2.push(Widget(10));

  BOOST_CHECK(st1 != st2); // Stack order matters for equality
}

BOOST_AUTO_TEST_CASE(Stack_Inequality_DifferentPartialStacks_DifferentCount) {
  stack<Widget> st1;
  st1.push(Widget(10));
  st1.push(Widget(20));

  stack<Widget> st2;
  st2.push(Widget(10)); // Fewer elements

  BOOST_CHECK(st1 != st2);
}

BOOST_AUTO_TEST_CASE(Stack_Equality_IdenticalFullStacks) {
  stack<Widget> st1;
  st1.push(Widget(1));
  st1.push(Widget(2));
  st1.push(Widget(3)); // Full (Cap 3)

  stack<Widget> st2;
  st2.push(Widget(1));
  st2.push(Widget(2));
  st2.push(Widget(3)); // Full (Cap 3)

  BOOST_CHECK(st1 == st2);
}

BOOST_AUTO_TEST_CASE(Stack_Inequality_DifferentFullStacks) {
  stack<Widget> st1;
  st1.push(Widget(1));
  st1.push(Widget(2));
  st1.push(Widget(3)); // Full (Cap 3)

  stack<Widget> st2;
  st2.push(Widget(1));
  st2.push(Widget(2));
  st2.push(Widget(4)); // Full (Cap 3), different element

  BOOST_CHECK(st1 != st2);
}

// --- Destructor Test ---
// Destructor is implicitly tested at the end of every test case
// when stack objects go out of scope. A explicit test is hard
// unless the Widget has side effects you can detect (like a counter).
// The existing test `Destructor` is sufficient for basic check.

BOOST_AUTO_TEST_CASE(Stack_Destructor_CleansUp) {
  // This test relies on Widget having a way to signal destruction,
  // e.g., incrementing a counter. Assuming Widget(int) constructor increments
  // a static counter and destructor decrements it.
  // This requires modification to the Widget class and potentially a helper function.
  // Without modification to Widget, we can't explicitly test memory cleanup beyond
  // the object going out of scope.
  // We'll keep the simple pop-based test as a sanity check.
  stack<Widget> st;
  st.push(Widget(1));             // Widget(1) created
  BOOST_CHECK_NO_THROW(st.pop()); // Widget(1) should be destroyed here
                                  // If Widget tracked creations/destructions, you'd check here.
                                  // e.g., BOOST_CHECK_EQUAL(Widget::get_instance_count(), 0);
}

// --- Test with a Different Type (e.g., int) ---
// This ensures the template works for different data types.

BOOST_AUTO_TEST_CASE(Stack_Int_BasicPushPop) {
  stack<int> st;
  BOOST_CHECK(st.empty());
  BOOST_CHECK_EQUAL(st.count(), 0);

  st.push(100);
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.top(), 100);
  BOOST_CHECK(!st.empty());

  st.push(200);
  BOOST_CHECK_EQUAL(st.count(), 2);
  BOOST_CHECK_EQUAL(st.top(), 200);

  st.pop();
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.top(), 100);

  st.pop();
  BOOST_CHECK_EQUAL(st.count(), 0);
  BOOST_CHECK(st.empty());

  BOOST_CHECK_THROW(st.top(), const char *);
  BOOST_CHECK_THROW(st.pop(), const char *);
}

BOOST_AUTO_TEST_CASE(Stack_Int_Resize) {
  stack<int> st; // Cap 1
  st.push(1);    // Count 1, Cap 1
  BOOST_CHECK_EQUAL(st.count(), 1);
  BOOST_CHECK_EQUAL(st.capacity(), 1);

  st.push(2); // Count 2, Cap 3 (assuming resize logic is consistent)
  BOOST_CHECK_EQUAL(st.count(), 2);
  BOOST_CHECK_EQUAL(st.capacity(), 3);
  BOOST_CHECK_EQUAL(st.top(), 2);

  st.push(3); // Count 3, Cap 3
  BOOST_CHECK_EQUAL(st.count(), 3);
  BOOST_CHECK_EQUAL(st.capacity(), 3);
  BOOST_CHECK_EQUAL(st.top(), 3);

  st.push(4); // Count 4, Cap 6 (assuming resize logic)
  BOOST_CHECK_EQUAL(st.count(), 4);
  BOOST_CHECK(st.capacity() >= 6);
  BOOST_CHECK_EQUAL(st.top(), 4);
}

BOOST_AUTO_TEST_SUITE_END()