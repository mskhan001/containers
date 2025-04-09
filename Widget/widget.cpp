#include <iostream>

#define WIDGET_TRACE()                                             \
    {                                                              \
        std::cout << __PRETTY_FUNCTION__ << " [x=" << x_ << "]\n"; \
    }

/*
helper class to test the functionality of containers
throws error on value 43
this is to test the exception handling
*/
class Widget
{
public:
    Widget(int x = 0) : x_(x)
    {

        WIDGET_TRACE();
        if (x_ == 43)
            throw std::invalid_argument("Invalid value: 43");
    }

    Widget(const Widget &other) : x_(other.x_) { WIDGET_TRACE(); }
    Widget(Widget &&other) noexcept : x_(other.x_) { WIDGET_TRACE(); }
    Widget &operator=(const Widget &other)
    {
        x_ = other.x_;
        WIDGET_TRACE();
        return *this;
    }
    Widget &operator=(Widget &&other) noexcept
    {
        x_ = other.x_;
        WIDGET_TRACE();
        return *this;
    }
    ~Widget() { WIDGET_TRACE(); }

    void doSomething() const { WIDGET_TRACE(); }

    friend bool operator==(const Widget &lhs, const Widget &rhs)
    {
        return lhs.x_ == rhs.x_;
    }

    friend bool operator!=(const Widget &lhs, const Widget &rhs)
    {
        return lhs.x_ != rhs.x_;
    }

    friend bool operator<(const Widget &lhs, const Widget &rhs)
    {
        return lhs.x_ < rhs.x_;
    }

    friend bool operator>(const Widget &lhs, const Widget &rhs)
    {
        return lhs.x_ > rhs.x_;
    }

    friend std::ostream &operator<<(std::ostream &os, const Widget &w)
    {
        os << "Widget(" << w.x_ << ")";
        return os;
    }

private:
    int x_;
};
