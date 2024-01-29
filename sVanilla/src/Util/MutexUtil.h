#pragma once

#include <mutex>
#include <shared_mutex>

namespace until
{
class hysteretic_shared_mutex
{
public:
    void lock()
    {
        std::unique_lock locker(m_mutex);
        ++m_writeWaitCount;
        m_writeCondition.wait(locker, [this](){
            return m_readCount == 0;
        });
        --m_writeWaitCount;
        locker.release();
    }

    void lock_shared()
    {
        std::unique_lock locker(m_mutex);
        m_readCondition.wait(locker, [this](){
            return m_writeWaitCount == 0;
        });
        ++m_readCount;
    }

    void unlock()
    {
        size_t writeCount = m_writeWaitCount ;
        m_mutex.unlock();
        if (writeCount > 0)
        {
            m_writeCondition.notify_one();
        }
        else
        {
            m_readCondition.notify_all();
        }
    }

    void unlock_shared()
    {
        size_t writeCount = 0;
        {
            std::unique_lock locker(m_mutex);
            --m_readCount;
            writeCount = m_writeWaitCount;
        }
        if(writeCount > 0)
        {
            m_writeCondition.notify_one();
        }
        else
        {
            m_readCondition.notify_all();
        }
    }


private:
    std::mutex m_mutex;
    std::condition_variable m_readCondition;
    std::condition_variable m_writeCondition;
    size_t m_readCount{};
    size_t m_writeWaitCount{};
};


template<typename T, typename GUARD = std::lock_guard<std::mutex>>
class guard_proxy;


template<typename T,typename GUARD = std::lock_guard<std::mutex>>
class basic_mutexed
{
public:											 
    using lock_type = GUARD;
    using mutex_type = typename lock_type::mutex_type;
    using vale_type = T;
protected:
    T m_value;
    mutable mutex_type m_mutex;
public:
    template<typename ...Args>
    basic_mutexed(Args &&...args) :m_value(std::forward<Args>(args)...)
    {}


    guard_proxy<T, lock_type> operator->()
    {
        return get();
    }

    guard_proxy<const T, lock_type> operator->()const
    {
        return get();
    }

    template<typename LockType = lock_type>
    guard_proxy<T, LockType > get()
    {
        return {&m_value,m_mutex};
    }

    template<typename LockType = lock_type>
    guard_proxy<const T, LockType > get()const
    {
        return { &m_value,m_mutex };
    }
    
    mutex_type& mutex()
    {
        return m_mutex;
    }
};

//real locked object
template<typename T, typename GUARD >
class guard_proxy
{
    template<typename, typename >
    friend class detail::basic_mutexed;
    template<typename, typename>
    friend class mutexed;
    template<typename, typename>
    friend class guard_proxy;

public:
    using value_type = T;
    using lock_type = GUARD;
    using mutex_type = typename lock_type::mutex_type;
private:
    T *m_value = {};
    lock_type m_guard;
public:
    guard_proxy() = default;
    guard_proxy(guard_proxy &&) = default;

    template<typename TT, typename GGUARD
        , std::enable_if_t<!std::is_same_v<guard_proxy<TT, GGUARD>, guard_proxy<T, GUARD>>> * = nullptr>
    guard_proxy(guard_proxy<TT, GGUARD> &&other) :m_value(other.m_value), m_guard(std::move(other.m_guard))
    {}

    template<typename TT, typename GGUARD>
    guard_proxy(T *value, guard_proxy<TT, GGUARD> &&other)
        : m_value(value)
        , m_guard(std::move(other.m_guard))
    {}

    guard_proxy(T *value, mutex_type &m) :m_value(value), m_guard(m)
    {}

    lock_type &guard()
    {
        return m_guard;
    }

    std::remove_reference_t<T> *operator->()const
    {
        return &**this;
    }

    operator bool() const
    {
        return m_value;
    }

    T &operator*()const
    {
        return *m_value;
    }

};

//Variables that are strictly locked, can't be used without locking
template<typename T, typename GUARG = std::lock_guard<std::mutex>>
class mutexed:public detail::basic_mutexed<T, GUARG >
{
public:
    using base_type = detail::basic_mutexed<T, GUARG >;
    using lock_type = typename base_type::lock_type;
    using mutex_type = typename lock_type::mutex_type;
    using base_type::base_type;
};


template<typename T, typename MUTEX >
class mutexed<T, std::shared_lock<MUTEX>> :public detail::basic_mutexed<T, std::lock_guard<MUTEX>>
{
public:
    using base_type = detail::basic_mutexed<T, std::lock_guard<MUTEX>>;
    using lock_type = std::shared_lock<MUTEX>;
    using mutex_type = typename lock_type::mutex_type;
    using value_type = T;
public:
    using base_type::base_type;

    guard_proxy<T, lock_type> get_shared()
    {
        return { &this->m_value,base_type::m_mutex };
    }

    guard_proxy<const T, lock_type> get_shared()const
    {
        return { &this->m_value,base_type::m_mutex };
    }
};
}
