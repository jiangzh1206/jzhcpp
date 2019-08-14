#pragma once
#include <cassert>
#include <atomic>

namespace MCPP
{

// intrusive_ptr come from boost

template<class T> 
class intrusive_ptr
{
public:

    using element_type = T;
    using this_type = intrusive_ptr;

    intrusive_ptr() noexcept : m_ptr(nullptr) {}
    
    intrusive_ptr(T* p, bool add_ref = true)
    {
        m_ptr = p;
        if (m_ptr && add_ref) {
            intrusive_ptr_add_ref(m_ptr);
        }
    }

    intrusive_ptr(intrusive_ptr const& r)
    {
        m_ptr = r.m_ptr;
        intrusive_ptr_add_ref(m_ptr);
    }

    template<class Y> 
    intrusive_ptr(intrusive_ptr<Y> const& r)
    {
        m_ptr = r.get();
        if (m_ptr) {
            intrusive_ptr_add_ref(r);
        }
    }

    intrusive_ptr(intrusive_ptr&& r)
    {
        m_ptr = r.m_ptr;
        r.m_ptr = nullptr;
    }

    template<class U> friend class intrusive_ptr;

    template<class Y> 
    intrusive_ptr(intrusive_ptr<Y>&& r)
    {
        m_ptr = r.m_ptr;
        r.m_ptr = nullptr;
    }

    ~intrusive_ptr()
    {
        if (m_ptr) {
            intrusive_ptr_release(m_ptr);
        }
    }

    intrusive_ptr& operator=(intrusive_ptr const& r)
    {
        this_type(r).swap(*this);
        return *this;
    }
    template<class Y> 
    intrusive_ptr& operator=(intrusive_ptr<Y> const& r)
    {
        this_type(static_cast<intrusive_ptr<Y> &&>(r)).swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(T* r)
    {
        this_type(r).swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(intrusive_ptr&& r)
    {
        this_type(static_cast<intrusive_ptr&&>(r)).swap(*this);
        return *this;
    }

    template<class Y> 
    intrusive_ptr& operator=(intrusive_ptr<Y>&& r)
    {

        this_type(static_cast<intrusive_ptr<Y>&&>(r)).swap(*this);
        return *this;
    }

    void reset()
    {
        this_type().swap(*this);
    }

    void reset(T* r)
    {
        this_type(r).swap(*this);
    }

    void reset(T* r, bool add_ref)
    {
        this_type(r, add_ref).swap(*this);
    }

    T& operator*() const noexcept
    {
        assert(m_ptr != nullptr);
        return *m_ptr;
    }

    T* operator->() const noexcept
    {
        assert(m_ptr != nullptr);
        return m_ptr;
    }

    T* get() const noexcept
    {
        return m_ptr;
    }

    T* detach() noexcept
    {
        T* p = m_ptr;
        m_ptr = nullptr;
        return p;
    }

    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    void swap(intrusive_ptr& b) noexcept
    {
        T* p = m_ptr;
        m_ptr = b.m_ptr;
        b.m_ptr = p;
    }

private:
    T* m_ptr;
};

template<class T, class U>
inline bool operator==(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b) noexcept
{
    return a.get() == b.get();
}

template<class T, class U>
inline bool operator!=(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b) noexcept
{
    return a.get() != b.get();
}

template<class T, class U>
inline bool operator==(intrusive_ptr<T> const& a, U* b) noexcept
{
    return a.get() == b;
}

template<class T, class U>
inline bool operator!=(intrusive_ptr<T> const& a, U* b) noexcept
{
    return a.get() != b;
}

template<class T, class U>
inline bool operator==(T* a, intrusive_ptr<U> const& b) noexcept
{
    return a == b.get();
}

template<class T, class U>
inline bool operator!=(T* a, intrusive_ptr<U> const& b) noexcept
{
    return a != b.get();
}

template<class T>
inline bool operator<(intrusive_ptr<T> const& a, intrusive_ptr<T> const& b) noexcept
{
    return a.get() < b.get();
}

template<class T> 
inline void swap(intrusive_ptr<T>& a, intrusive_ptr<T>& b) noexcept
{
    a.swap(b);
}

template<class T, class U>
inline intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const& p) noexcept
{
    return static_cast<T *>(p.get());
}

template<class T, class U>
inline intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const& p) noexcept
{
    return const_cast<T *>(p.get());
}

template<class T, class U>
inline intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const& p) noexcept
{
    return dynamic_cast<T *>(p.get());
}

template<class E, class T, class Y>
std::basic_ostream<E, T>& operator<< (std::basic_ostream<E, T>& os, intrusive_ptr<Y> const& p)
{
    os << p.get();
    return os;
}

class thread_safe_count
{
public:
    using type = std::atomic_uint;

    static unsigned int load(std::atomic_uint const& counter) noexcept
    {
        return counter;
    }

    static void increment(std::atomic_uint& counter) noexcept
    {
        ++counter;
    }

    static unsigned int decrement(std::atomic_uint& counter) noexcept
    {
        return --counter;
    }
};

class thread_unsafe_count
{
public:
    using type = unsigned int;

    static unsigned int load(unsigned int const& counter) noexcept
    {
        return counter;
    }

    static void increment(unsigned int& counter) noexcept
    {
        ++counter;
    }

    static unsigned int decrement(unsigned int& counter) noexcept
    {
        return --counter;
    }
};

// class myobj : public intrusive_base
// {
//     ...
// };
// intrusive_ptr<myobj> sp(myobj());

template<typename T, typename CounterPolicyT = thread_safe_count>
class intrusive_base
{
    friend void intrusive_ptr_release(intrusive_base* obj)
    {
        CounterPolicyT::increment(obj->m_ref_counter);
    }

    friend void intrusive_ptr_add_ref(intrusive_base* obj)
    {
        if (CounterPolicyT::decrement(obj->m_ref_counter) == 0) {
            delete static_cast<T*>(obj);
        }
    }

    using count_type = typename CounterPolicyT::type;
    mutable count_type m_ref;

protected:
    intrusive_base() : m_ref(0) {}

public:
    unsigned int use_count() const noexcept
    {
        return CounterPolicyT::load(m_ref);
    }
};

}