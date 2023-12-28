
#include <array>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <type_traits>
#include <hng/defer/defer.h>

namespace hng {
    namespace defer_tests {

        template<class F>
        bool test(char const*const name, F&& f) {
            bool success = false;
            try {
                success = std::invoke(std::forward<F>(f), std::as_const(name));
            }
            catch (...) {
                std::cerr << "Test failed: Test named \"" << name << "\" failed" << std::endl;
                return false;
            }
            if (!success) {
                std::cerr << "Test failed: Test named \"" << name << "\" failed" << std::endl;
            }
            return success;
        }

        void run_tests() {
            std::vector<std::function<bool()>> tests;

            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - try block success", [](auto const& /*test_name*/) {
                {
                    int x = 1;
                    int xd = 0;
                    HNG_DT_DEFER_FINALLY[&]
                    {
                        xd = x;
                        x = 0;
                    }
                        HNG_DT_TRY[&]
                    {
                        x = 2;
                    }
                    HNG_DT_END;
                    return x == 0 && xd == 2;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - try block exception", [](auto const& /*test_name*/) {
                {
                    int x = 1;
                    try {
                        HNG_DT_DEFER_FINALLY[&]
                        {
                            x = 0;
                        }
                            HNG_DT_TRY[&]
                        {
                            throw std::runtime_error("test");
                        }
                        HNG_DT_END;
                        return false;
                    }
                    catch (std::runtime_error const& ex) {
                        if (!(0 == std::strcmp(ex.what(), "test")))
                            return false;
                    }
                    return x == 0;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - try block return", [](auto const& /*test_name*/) {
                {
                    int x = 1;
                    long y = HNG_DT_DEFER_FINALLY[&]
                    {
                        x = 0;
                    }
                        HNG_DT_TRY[&]
                    {
                        return 2L;
                    }
                    HNG_DT_END;
                    return x == 0 && y == 2L;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - nested defer", [](auto const& /*test_name*/) {
                {
                    auto a = std::vector<int>();
                    HNG_DT_DEFER_FINALLY[&]
                    {
                        HNG_DT_DEFER_FINALLY[&]
                        {
                            a.push_back(3);
                        }
                        HNG_DT_TRY[&]
                        {
                            a.push_back(2);
                        }
                        HNG_DT_END;
                    }
                        HNG_DT_TRY[&]
                    {
                        a.push_back(1);
                    }
                    HNG_DT_END;
                    a.push_back(4);
                    std::vector<int> expected{ 1, 2, 3, 4 };
                    return a == expected;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - expression return value", [](auto const& /*test_name*/) {
                {
                    int x = 0;
                    int y = 1;
                    y = HNG_DT_DEFER_FINALLY[&]
                    {
                        // executed after the return statement in the try block.
                        x = y;
                    }
                        HNG_DT_TRY[&]
                    {
                        return 2;
                    }
                    HNG_DT_END;
                    return x == 1 && y == 2;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - expression return value by move", [](auto const& /*test_name*/) {
                {
                    std::unique_ptr<int> x = std::make_unique<int>(3);
                    int z = 1;
                    std::unique_ptr<int> r = HNG_DT_DEFER_FINALLY[&]
                    {
                        // executed after the return statement in the try block.
                        z = 2;
                    }
                        HNG_DT_TRY[&]() -> std::unique_ptr<int>&&
                    {
                        return std::move(x);
                    }
                    HNG_DT_END;
                    return z == 2 && r && *r == 3;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY/TRY/END macros - expression return value by reference", [](auto const& /*test_name*/) {
                {
                    std::unique_ptr<int> x = std::make_unique<int>(3);
                    int z = 1;
                    std::unique_ptr<int> const& r = HNG_DT_DEFER_FINALLY[&]
                    {
                        // executed after the return statement in the try block.
                        z = 2;
                    }
                        HNG_DT_TRY[&]() -> std::unique_ptr<int>&
                    {
                        return (x);
                    }
                    HNG_DT_END;
                    return z == 2 && r && *r == 3 && &r == &x;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY_PRESERVE/TRY/END macros - try block exception", [](auto const& /*test_name*/) {
                {
                    int x = 1;
                    try {
                        HNG_DT_DEFER_FINALLY_PRESERVE[&]
                        {
                            x = 0;
                        }
                            HNG_DT_TRY[&]
                        {
                            throw std::runtime_error("test");
                        }
                        HNG_DT_END;
                    }
                    catch (std::runtime_error const& ex) {
                        if (!(0 == std::strcmp(ex.what(), "test")))
                            return false;
                    }
                    return x == 0;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY_PRESERVE/TRY/END macros - defer block exception", [](auto const& /*test_name*/) {
                {
                    int x = 1;
                    try {
                        HNG_DT_DEFER_FINALLY_PRESERVE[&]
                        {
                            throw std::runtime_error("test");
                        }
                            HNG_DT_TRY[&]
                        {
                            x = 2;
                        }
                        HNG_DT_END;
                    }
                    catch (std::runtime_error const& ex) {
                        if (!(0 == std::strcmp(ex.what(), "test")))
                            return false;
                    }
                    return x == 2;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DT DEFER_FINALLY_PRESERVE/TRY/END macros - try block exception and defer block exception", [](auto const& /*test_name*/) {
                {
                    try {
                        HNG_DT_DEFER_FINALLY_PRESERVE[&]
                        {
                            throw std::runtime_error("e2");
                        }
                            HNG_DT_TRY[&]
                        {
                            throw std::runtime_error("e1");
                        }
                        HNG_DT_END;
                    }
                    catch (hng::defer_exception const& dex) {
                        try {
                            std::rethrow_exception(std::move(dex.exception_ptr()));
                        }
                        catch (std::runtime_error const& ex) {
                            if (!(0 == std::strcmp(ex.what(), "e2")))
                                return false;
                        }
                        try {
                            std::rethrow_if_nested(dex);
                        }
                        catch (std::runtime_error const& nex) {
                            if (!(0 == std::strcmp(nex.what(), "e1")))
                                return false;
                        }
                    }
                    return true;
                }
                }); });
            tests.emplace_back([] { return test("defer class - basic", [](auto const& /*test_name*/) {
                {
                    int* raw_ptr = new int(4);
                    {
                        auto const my_callable = [&]()noexcept {
                            delete raw_ptr;
                            raw_ptr = nullptr;
                            };
                        hng::defer<decltype(my_callable)> const my_defer(my_callable);

                        if (!(raw_ptr && *raw_ptr == 4))
                            return false;
                    }
                    return raw_ptr == nullptr;
                }
                }); });

#if DETAIL_HNG_DEFER_HAS_CPP17
            tests.emplace_back([] { return test("defer class - type argument inference", [](auto const& /*test_name*/) {
                {
                    int* raw_ptr = new int(5);
                    {
                        auto const my_defer = hng::defer([&]()noexcept {
                            delete raw_ptr;
                            raw_ptr = nullptr;
                            });

                        if (!(raw_ptr && *raw_ptr == 5))
                            return false;
                    }
                    return raw_ptr == nullptr;
                }
                }); });
#endif // ^^^^ DETAIL_HNG_DEFER_HAS_CPP17

#if DETAIL_HNG_DEFER_HAS_CPP17
            tests.emplace_back([] { return test("HNG_DEFER BEGIN/END macros", [](auto const& /*test_name*/) {
                {
                    int* raw_ptr = new int(3);
                    {
                        HNG_DEFER_BEGIN
                        {
                            delete raw_ptr;
                            raw_ptr = nullptr;
                        }
                        HNG_DEFER_END;

                        if (!(raw_ptr && *raw_ptr == 3))
                            return false;
                    }
                    return raw_ptr == nullptr;
                }
                }); });
#endif // ^^^^ DETAIL_HNG_DEFER_HAS_CPP17

            tests.emplace_back([] { return test("HNG_DEFER_BLOCK macro", [](auto const& /*test_name*/) {
                {
                    int* raw_ptr = new int(2);
                    {
                        HNG_DEFER_BLOCK({
                            delete raw_ptr;
                            raw_ptr = nullptr;
                            });

                        if (!(raw_ptr && *raw_ptr == 2))
                            return false;
                    }
                    return raw_ptr == nullptr;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DEFER_NAMED BEGIN/END macros", [](auto const& /*test_name*/) {
                {
                    int* raw_ptr = new int(1);
                    {
                        HNG_DEFER_NAMED_BEGIN(my_name)
                        {
                            delete raw_ptr;
                            raw_ptr = nullptr;
                        }
                        HNG_DEFER_NAMED_END(my_name);

                        if (!(raw_ptr && *raw_ptr == 1))
                            return false;
                    }
                    return raw_ptr == nullptr;
                }
                }); });
            tests.emplace_back([] { return test("HNG_DEFER_CALLABLE_VARIABLE macro", [](auto const& /*test_name*/) {
                {
                    int* raw_ptr = new int(-3);
                    {
                        auto my_callable = [&]()noexcept {
                            delete raw_ptr;
                            raw_ptr = nullptr;
                            };
                        HNG_DEFER_CALLABLE_VARIABLE(my_callable);

                        if (!(raw_ptr && *raw_ptr == -3))
                            return false;
                    }
                    return raw_ptr == nullptr;
                }
                }); });



            bool all = true;
            for (auto& test : tests) {
                if (!std::invoke(std::move(test))) {
                    all = false;
                }
            }
            if (!all) {
                throw std::runtime_error("Some tests failed");
            }
        }
    }
}

int main() {
    try {
        hng::defer_tests::run_tests();
        std::cout << "All tests passed." << std::endl;
    }
    catch (std::exception const& ex) {
        std::cerr << "Testing failed: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Testing failed" << std::endl;
    }
}
