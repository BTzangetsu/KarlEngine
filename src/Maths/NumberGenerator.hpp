#pragma once
#include <random>
#include <iostream>
#include <cassert>
#include <stdexcept>

namespace KE {
    template<typename T>
    class NumGenerator {
    private:
        T MIN;  // Minimum boundary for generation
        T MAX;  // Maximum boundary for generation
        std::mt19937 gen;  // Mersenne Twister random number generator (stored to avoid recreation)

    public:
        // Constructor: initializes the generator with min and max boundaries
        NumGenerator(T min, T max) : MIN(min), MAX(max), gen(std::random_device{}()) {
            if (min > max) {
                throw std::invalid_argument("The min is greater than the max");
            }
        }

        // Copy constructor
        NumGenerator(const NumGenerator& other)
            : MIN(other.MIN), MAX(other.MAX), gen(std::random_device{}()) {
        }

        // Generate a random number using the stored MIN and MAX boundaries
        T generate() {
            if constexpr (std::is_integral_v<T>) {
                // Use integer distribution for integral types
                std::uniform_int_distribution<T> distrib(MIN, MAX);
                return distrib(gen);
            }
            else {
                // Use real distribution for floating-point types
                std::uniform_real_distribution<T> distrib(MIN, MAX);
                return distrib(gen);
            }
        }

       

        // Static method: generate a random number without creating an object
        static T generate(T min, T max) {
            if (min > max) {
                throw std::invalid_argument("The min is greater than the max");
            }

            std::random_device rd;
            std::mt19937 tempGen(rd());

            if constexpr (std::is_integral_v<T>) {
                std::uniform_int_distribution<T> distrib(min, max);
                return distrib(tempGen);
            }
            else {
                std::uniform_real_distribution<T> distrib(min, max);
                return distrib(tempGen);
            }
        }

        // Set new boundaries for the generator
        void setLimits(T min, T max) {
            if (min > max) {
                throw std::invalid_argument("The min is greater than the max");
            }
            MIN = min;
            MAX = max;
        }

        // Get the current minimum boundary
        T getMinLimit() const {
            return MIN;
        }

        // Get the current maximum boundary
        T getMaxLimit() const {
            return MAX;
        }
    };
}

/*
 * USAGE EXAMPLES & PERFORMANCE TIPS
 *
 * ====================================
 * RECOMMENDED USAGE (Most Efficient):
 * ====================================
 *
 * KE::NumGenerator<int> gen(0, 100);
 *
 * // Generate multiple numbers with the same limits - FAST!
 * for (int i = 0; i < 1000; i++) {
 *     int random = gen.generate();  // Uses stored generator - very efficient
 * }
 *
 * // Change limits when needed
 * gen.setLimits(200, 300);
 * for (int i = 0; i < 1000; i++) {
 *     int random = gen.generate();  // Still efficient with new limits
 * }
 *
 *
 * ====================================
 * LESS EFFICIENT (Use Sparingly):
 * ====================================
 *
 * // Passing temporary limits recreates the generator each time - SLOWER!
 * for (int i = 0; i < 1000; i++) {
 *     int random = gen.generate(50, 75);  //  Creates new random_device and mt19937 every call
 * }
 *
 * // Static method also recreates generator each time - SLOWER!
 * for (int i = 0; i < 1000; i++) {
 *     int random = KE::NumGenerator<int>::generate(10, 20);  //  Inefficient in loops
 * }
 *
 *
 * ====================================
 * WHEN TO USE TEMPORARY LIMITS:
 * ====================================
 *
 * // Good: One-off generation with different limits
 * int score = gen.generate();           // 0-100
 * int bonus = gen.generate(1, 10);      // 1-10 (just once)
 * int multiplier = gen.generate(2, 5);  // 2-5 (just once)
 *
 * // Bad: Same limits in a loop
 * for (int i = 0; i < 1000; i++) {
 *     int x = gen.generate(0, 10);  //  Don't do this!
 * }
 *
 * // Good: Use setLimits instead
 * gen.setLimits(0, 10);
 * for (int i = 0; i < 1000; i++) {
 *     int x = gen.generate();  //  Much faster!
 * }
 *
 *
 * ====================================
 * PERFORMANCE SUMMARY:
 * ====================================
 *
 * generate()              -> FAST  (reuses stored mt19937 generator)
 * generate(min, max)      -> SLOW  (creates new random_device + mt19937 each call)
 * ::generate(min, max)    -> SLOW  (creates new random_device + mt19937 each call)
 *
 * TIP: For generating many random numbers, always prefer:
 *      1. Create NumGenerator object
 *      2. Call setLimits() when boundaries change
 *      3. Call generate() without parameters
 */