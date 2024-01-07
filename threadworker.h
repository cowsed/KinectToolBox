#pragma once
#include <concepts>
#include <functional>
#include <future>
#include <iostream>
#include <utility>
template<typename R, typename... Args>
class ThreadWorker
{
public:
    using FType = std::function<R(Args...)>;
    ThreadWorker(FType func, R def, std::function<void(void)> on_finish)
        : func(func)
        , still_working(false)
        , res(def)
        , on_finish(std::move(on_finish))
    {}

    void add_work(Args... args)
    {
        if (still_working) {
            std::cout << "Trying to add data while its still working. discarding this set"
                      << std::endl;
            return;
        }
        still_working = true;
        fut = std::async(std::launch::async, [args..., this]() {
            auto returned = func(args...);
            (void) get_result();
            on_finish();

            return returned;
        });
    }

    R get_result()
    {
        std::chrono::system_clock::time_point two_seconds_passed = std::chrono::system_clock::now()
                                                                   + std::chrono::microseconds(2);

        if (fut.valid() && still_working) {
            auto stat = fut.wait_for(std::chrono::microseconds(1));
            if (stat == std::future_status::ready) {
                res = fut.get();
                still_working = false;
            }
        }
        return res;
    }

private:
    std::future<R> fut;
    bool still_working;
    R res;
    FType func;
    std::function<void(void)> on_finish;
};
