#ifndef MULTOBJOPT_TEST_SUPPORT_HPP
#define MULTOBJOPT_TEST_SUPPORT_HPP

#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <source_location>
#include <string_view>
#include <utility>

namespace test_support {

class test_context {
public:
    void check(
        bool condition,
        std::string_view message,
        const std::source_location location = std::source_location::current()) {
        if (!condition) {
            report(message, location);
        }
    }

    void check_near(
        double actual,
        double expected,
        double tolerance,
        std::string_view message,
        const std::source_location location = std::source_location::current()) {
        if (!(std::abs(actual - expected) <= tolerance)) {
            std::cerr << location.file_name() << ':' << location.line()
                      << ": check failed: " << message << " (actual=" << actual
                      << ", expected=" << expected << ", tolerance=" << tolerance
                      << ")\n";
            ++failure_count_;
        }
    }

    template <typename exception_type, typename function_type>
    void check_throws(
        function_type&& function,
        std::string_view message,
        const std::source_location location = std::source_location::current()) {
        try {
            std::forward<function_type>(function)();
        } catch (const exception_type&) {
            return;
        } catch (const std::exception& exception) {
            std::cerr << location.file_name() << ':' << location.line()
                      << ": check failed: " << message
                      << " (unexpected exception: " << exception.what() << ")\n";
            ++failure_count_;
            return;
        } catch (...) {
            std::cerr << location.file_name() << ':' << location.line()
                      << ": check failed: " << message
                      << " (unexpected non-standard exception)\n";
            ++failure_count_;
            return;
        }
        report(message, location);
    }

    [[nodiscard]] int finish() const {
        if (failure_count_ != 0) {
            std::cerr << failure_count_ << " check(s) failed\n";
            return 1;
        }
        return 0;
    }

private:
    void report(
        std::string_view message,
        const std::source_location& location) {
        std::cerr << location.file_name() << ':' << location.line()
                  << ": check failed: " << message << '\n';
        ++failure_count_;
    }

    std::size_t failure_count_{0};
};

} // namespace test_support

#endif
