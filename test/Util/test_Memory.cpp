#include <catch2/catch.hpp>
#include <Euclid/Util/Memory.h>

#ifndef NDEBUG
#include <iostream>
#endif

class Base
{
public:
    explicit Base(int d) : data(d)
    {
#ifndef NDEBUG
        std::cout << "Base: " << data << std::endl;
#endif
    }

    virtual ~Base()
    {
#ifndef NDEBUG
        std::cout << "~Base: " << data << std::endl;
#endif
    }

    int data;
};

class Derived : public Base
{
public:
    explicit Derived(int d) : Base(d)
    {
#ifndef NDEBUG
        std::cout << "Derived: " << data << std::endl;
#endif
    }

    ~Derived()
    {
#ifndef NDEBUG
        std::cout << "~Derived: " << data << std::endl;
#endif
    }

    int get() const
    {
        return this->data;
    }
};

TEST_CASE("Util, Memory", "[util][memory]")
{
    using BPtr = Euclid::ProPtr<Base>;
    using DPtr = Euclid::ProPtr<Derived>;

    SECTION("ctor default")
    {
        BPtr p;
        REQUIRE(!p);
        REQUIRE(p.get() == nullptr);
        REQUIRE(!p.owns());
    }

    SECTION("ctor nullptr")
    {
        BPtr p(nullptr);
        REQUIRE(!p);
        REQUIRE(p.get() == nullptr);
        REQUIRE(!p.owns());
    }

    SECTION("ctor data")
    {
        Base b1{ 1 };
        auto b2 = new Base(2);

        // non-owning automatic variable
        {
            BPtr p1(&b1, false);
            REQUIRE(p1);
            REQUIRE(p1.get() == &b1);
            REQUIRE(!p1.owns());
            REQUIRE(p1->data == 1);
            REQUIRE((*p1).data == 1);
        }

        // non-owning dynamic memory
        {
            BPtr p2(b2, false);
            REQUIRE(p2);
            REQUIRE(p2.get() == b2);
            REQUIRE(!p2.owns());
            REQUIRE(p2->data == 2);
            REQUIRE((*p2).data == 2);
        }
        REQUIRE(b2->data == 2);

        // owning dynamic memory
        {
            BPtr p3(b2, true);
            REQUIRE(p3);
            REQUIRE(p3.get() == b2);
            REQUIRE(p3.owns());
            REQUIRE(p3->data == 2);
            REQUIRE((*p3).data == 2);
        }
    }

    SECTION("copy and move")
    {
        // move assignment
        auto p1 = BPtr(new Base(1), true);
        REQUIRE(p1);

        // copy from nullptr
        p1 = nullptr;
        REQUIRE(!p1);

        // move construct
        auto p2(BPtr(new Base(2), true));
        REQUIRE(p2);

        // move from self
        p2 = std::move(p2);
        REQUIRE(p2);

        // move from existing
        p1 = std::move(p2);
        REQUIRE(p1);
        REQUIRE(!p2);
    }

    SECTION("move from derived type")
    {
        auto p1 = BPtr(new Base(1), true);
        auto p2 = DPtr(new Derived(2), true);
        REQUIRE(p2->get() == 2);

        p1 = std::move(p2);
        REQUIRE(p1->data == 2);
        REQUIRE(dynamic_cast<Derived*>(p1.get())->get() == 2);

        p1 = DPtr(new Derived(3), true);
        REQUIRE(p1->data == 3);
        REQUIRE(dynamic_cast<Derived*>(p1.get())->get() == 3);
    }

    SECTION("release and reset")
    {
        BPtr p1(new Base(1), true);
        BPtr p2(new Base(2), true);

        auto tmp = p1.release();
        REQUIRE(tmp->data == 1);

        p2.reset(tmp, true);
        REQUIRE(p2->data == 1);
    }

    SECTION("swap")
    {
        BPtr p1(new Base(1), true);
        BPtr p2(new Base(2), true);
        BPtr p3(p2.get(), false);

        Euclid::swap(p1, p1);
        REQUIRE(p1->data == 1);

        Euclid::swap(p1, p3);
        REQUIRE(p1->data == 2);
        REQUIRE(!p1.owns());
        REQUIRE(p1.get() == p2.get());
        REQUIRE(p3->data == 1);
        REQUIRE(p3.owns());
    }
}
