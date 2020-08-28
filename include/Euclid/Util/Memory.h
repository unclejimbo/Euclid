/** Memory utilities.
 *
 *  This pacakge provides more memory managing facilities.
 *  @defgroup PkgMemory Memory
 *  @ingroup PkgUtil
 */
#pragma once

#include <memory>
#include <utility>

namespace Euclid
{
/**@{*/

/** A potentially resource owning pointer.
 *
 *  This class wraps up raw pointers who might or might not own the underlying
 *  resource. The motivation of this design is that when constructing complex
 *  algorithms one may need to use a lot of components, and some of them may
 *  share commonn internal computations. So it's desired that they are computed
 *  once and used everywhere as long as the user kept the underlying resource
 *  valid. This class will automatically manage the resource in one place and
 *  free the data when it goes out of scope.
 */
template<typename T>
class ProPtr
{
public:
    using pointer = T*;
    using element_type = T;

public:
    /** Default constructor.
     *
     */
    constexpr ProPtr() noexcept : _data(nullptr), _own(false) {}

    /** Construct from nullptr_t.
     *
     */
    constexpr ProPtr(std::nullptr_t) noexcept : _data(nullptr), _own(false) {}

    /** Construct from pointer
     *
     *  @param p The raw pointer.
     *  @param own Whether to own the underlying memory. Default is false and
     *  this class acts just like a raw pointer. Set to true to act like a
     *  unique_ptr.
     */
    explicit ProPtr(pointer p, bool own = false) noexcept : _data(p), _own(own)
    {}

    /** Non-copyable.
     *
     */
    ProPtr(const ProPtr& pro) = delete;

    /** Move constructor.
     *
     */
    ProPtr(ProPtr&& pro) noexcept
    {
        pro.swap(*this);
    }

    /** Move construct from a derived type.
     *
     */
    template<typename U>
    ProPtr(ProPtr<U>&& pro) noexcept
    {
        auto owning = pro.owns();
        auto data = pro.release();
        ProPtr<T> tmp(data, owning);
        tmp.swap(*this);
    }

    /** Desctructor.
     *
     *  Free the memory if owns.
     */
    ~ProPtr()
    {
        if (_own) {
            delete _data;
        }
    }

    /** Non-copyable.
     *
     */
    ProPtr& operator=(const ProPtr& pro) = delete;

    /** Copy assignment from nullptr.
     *
     *  Equals to call reset(nullptr).
     */
    ProPtr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    /** Move assignment operator.
     *
     */
    ProPtr& operator=(ProPtr&& pro) noexcept
    {
        pro.swap(*this);
        return *this;
    }

    /** Move assignment from a derived type.
     *
     */
    template<typename U>
    ProPtr& operator=(ProPtr<U>&& pro) noexcept
    {
        auto owning = pro.owns();
        auto data = pro.release();
        ProPtr<T> tmp(data, owning);
        tmp.swap(*this);
        return *this;
    }

    /** Release the ownership(if owns) and return the data.
     *
     */
    pointer release() noexcept
    {
        pointer result = nullptr;
        std::swap(result, _data);
        _own = false;
        return result;
    }

    /** Reset the ProPtr.
     *
     *  **Note**
     *
     *  Self-reset is not checked. Double owning is not checked either.
     *  Proceed with caution.
     */
    void reset(pointer p = pointer(), bool own = false) noexcept
    {
        if (_own) {
            delete _data;
        }
        _data = p;
        _own = own;
    }

    /** Swap this ProPtr with another, both resource and ownership.
     *
     */
    void swap(ProPtr& pro) noexcept
    {
        using std::swap;
        swap(pro._data, _data);
        swap(pro._own, _own);
    }

    /** Get the underlying resource.
     *
     */
    pointer get() const noexcept
    {
        return _data;
    }

    /** Return true if resource is valid.
     *
     */
    explicit operator bool() const noexcept
    {
        return _data;
    }

    /** Return the data being pointed to.
     *
     */
    typename std::add_lvalue_reference<T>::type operator*() const
    {
        return *_data;
    }

    /** Return the raw pointer.
     *
     */
    pointer operator->() const noexcept
    {
        return _data;
    }

    /** Return whether this pointer owns the resource.
     *
     */
    bool owns() const noexcept
    {
        return _own;
    }

private:
    pointer _data;
    bool _own;
};

/** Swap contents of two ProPtr<T>s.
 *
 */
template<typename T>
void swap(ProPtr<T>& lhs, ProPtr<T>& rhs)
{
    lhs.swap(rhs);
}

/**@}*/
} // namespace Euclid
