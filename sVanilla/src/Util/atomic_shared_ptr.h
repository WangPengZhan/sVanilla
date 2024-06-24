#include <atomic>
#include <memory>
#include <condition_variable>
#include <thread>
#include <version>

#if !defined(__cpp_lib_atomic_shared_ptr) || (__cpp_lib_atomic_shared_ptr == 0)
namespace std
{
template <typename T>
struct atomic<shared_ptr<T>>
{
    using value_type = shared_ptr<T>;
    static constexpr bool is_always_lock_free = false;
    bool is_lock_free() const noexcept
    {
        return false;
    }
    constexpr atomic() noexcept {};
    atomic(shared_ptr<T> desired) noexcept
        : ptr_(std::move(desired)){};
    atomic(const atomic&) = delete;
    void operator=(const atomic&) = delete;
    void store(shared_ptr<T> desired, memory_order order = memory_order::seq_cst) noexcept
    {
        std::lock_guard<std::mutex> lk(cv_m_);
        atomic_store_explicit(&ptr_, std::move(desired), order);
    }
    void operator=(shared_ptr<T> desired) noexcept
    {
        store(std::move(desired));
    }
    shared_ptr<T> load(memory_order order = memory_order::seq_cst) const noexcept
    {
        return atomic_load_explicit(&ptr_, order);
    }
    operator shared_ptr<T>() const noexcept
    {
        return load();
    }
    shared_ptr<T> exchange(shared_ptr<T> desired, memory_order order = memory_order::seq_cst) noexcept
    {
        return atomic_exchange_explicit(&ptr_, std::move(desired), order);
    }
    bool compare_exchange_weak(shared_ptr<T>& expected, shared_ptr<T> desired, memory_order success, memory_order failure) noexcept
    {
        return atomic_compare_exchange_weak_explicit(&ptr_, &expected, std::move(desired), success, failure);
    }
    bool compare_exchange_strong(shared_ptr<T>& expected, shared_ptr<T> desired, memory_order success, memory_order failure) noexcept
    {
        return atomic_compare_exchange_strong_explicit(&ptr_, &expected, std::move(desired), success, failure);
    }
    bool compare_exchange_weak(shared_ptr<T>& expected, shared_ptr<T> desired, memory_order order = memory_order::seq_cst) noexcept
    {
        return compare_exchange_weak(expected, std::move(desired), order, convert_order(order));
    }
    bool compare_exchange_strong(shared_ptr<T>& expected, shared_ptr<T> desired, memory_order order = memory_order::seq_cst) noexcept
    {
        return compare_exchange_strong(expected, std::move(desired), order, convert_order(order));
    }
    void wait(shared_ptr<T> old, memory_order order = memory_order::seq_cst) const noexcept
    {
        std::unique_lock<std::mutex> lk(cv_m_);
        cv_.wait(lk, [&] {
            return !(load(order) == old);
        });
    }
    void notify_one() noexcept
    {
        cv_.notify_one();
    }
    void notify_all() noexcept
    {
        cv_.notify_all();
    }

private:
    shared_ptr<T> ptr_;
    mutable std::condition_variable cv_;
    mutable std::mutex cv_m_;
    constexpr memory_order convert_order(memory_order order)
    {
        switch (order)
        {
        case std::memory_order_acq_rel:
            return std::memory_order_acquire;

        case std::memory_order_release:
            return std::memory_order_relaxed;
        default:
            return order;
        }
    }
};
}  // namespace std
#endif
