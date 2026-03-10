#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace KE {

    /**
     * @class RoundedRectShape
     * @brief A class for drawing a rectangle with rounded corners using SFML.
     *
     * This class inherits from `sf::Shape` and allows customization of size,
     * corner radius, and the number of points used to draw the rounded corners.
     *
     * Points are pre-computed for optimal performance.
     */
    class RoundedRectShape : public sf::Shape {
    public:

        /**
         * @brief Constructor with size and corner radius.
         * @param size The size of the rectangle (width, height).
         * @param radius The radius of the rounded corners.
         * @param pointCount Number of points per corner (total points = 4 * pointCount).
         */
        RoundedRectShape(const sf::Vector2f& size = sf::Vector2f(0.0f, 0.0f),
            float radius = 0.0f,
            unsigned int pointCount = 5)
            : m_size(size)
            , m_radius(radius)
            , m_pointCount(std::max(4u, pointCount * 4)) // Ensure multiple of 4
        {
            validateAndClampRadius();
            computePoints();
            update();
        }

        /**
         * @brief Default copy constructor and assignment operator.
         *
         * Since all members are copyable, we use the compiler-generated versions.
         */
        RoundedRectShape(const RoundedRectShape&) = default;
        RoundedRectShape& operator=(const RoundedRectShape&) = default;

        /**
         * @brief Default move constructor and assignment operator.
         */
        RoundedRectShape(RoundedRectShape&&) = default;
        RoundedRectShape& operator=(RoundedRectShape&&) = default;

        /**
         * @brief Default destructor.
         */
        ~RoundedRectShape() = default;

        /**
         * @brief Returns the total number of points defining the shape.
         * @return The total number of points used to draw the rounded rectangle.
         */
        std::size_t getPointCount() const override {
            return m_points.size();
        }

        /**
         * @brief Returns the position of a specific point in the shape.
         *
         * Points are pre-computed for performance.
         *
         * @param index The index of the point (must be between 0 and getPointCount() - 1).
         * @return The coordinates of the corresponding point.
         */
        sf::Vector2f getPoint(std::size_t index) const override {
            if (index >= m_points.size()) {
                return sf::Vector2f(0, 0);
            }
            return m_points[index];
        }

        /**
         * @brief Gets the radius of the rounded corners.
         * @return The radius of the corners.
         */
        float getRadius() const {
            return m_radius;
        }

        /**
         * @brief Sets the radius of the rounded corners and updates the shape.
         *
         * Radius is automatically clamped to not exceed half of the smaller dimension.
         *
         * @param radius The new radius value.
         */
        void setRadius(float radius) {
            m_radius = radius;
            validateAndClampRadius();
            computePoints();
            update();
        }

        /**
         * @brief Sets the total number of points used to draw the shape.
         *
         * The number of points will be adjusted to be a multiple of 4 to ensure
         * proper distribution across the rounded corners.
         *
         * @param pointCount The desired number of points per corner.
         */
        void setPointCount(unsigned int pointCount) {
            m_pointCount = std::max(4u, pointCount * 4);
            computePoints();
            update();
        }

        /**
         * @brief Sets the size of the rectangle and updates the shape.
         *
         * Radius is automatically clamped if it exceeds half of the new smaller dimension.
         *
         * @param size The new size of the rectangle (width, height).
         */
        void setSize(const sf::Vector2f& size) {
            m_size = size;
            validateAndClampRadius();
            computePoints();
            update();
        }

        /**
         * @brief Gets the current size of the rectangle.
         * @return The current size (width, height).
         */
        const sf::Vector2f& getSize() const {
            return m_size;
        }

    private:
        // Member variables with consistent naming (m_ prefix)
        sf::Vector2f m_size;                    ///< Size of the rectangle.
        float m_radius;                         ///< Radius of the rounded corners.
        unsigned int m_pointCount;              ///< Total number of points defining the shape.
        std::vector<sf::Vector2f> m_points;     ///< Pre-computed points for performance.

        /**
         * @brief Validates and clamps the radius to ensure it doesn't exceed constraints.
         *
         * The radius cannot be greater than half of the smaller dimension of the rectangle.
         */
        void validateAndClampRadius() {
            float maxRadius = std::min(m_size.x, m_size.y) / 2.0f;

            if (m_radius > maxRadius) {
                m_radius = maxRadius;
                sf::err() << "RoundedRectShape: Radius clamped to " << m_radius
                    << " (max allowed for size " << m_size.x << "x" << m_size.y << ")"
                    << std::endl;
            }

            // Ensure radius is non-negative
            if (m_radius < 0.0f) {
                m_radius = 0.0f;
            }
        }

        /**
         * @brief Pre-computes all points of the rounded rectangle.
         *
         * This avoids expensive trigonometric calculations during rendering.
         * Points are stored in clockwise order starting from top-left corner.
         */
        void computePoints() {
            m_points.clear();

            if (m_pointCount == 0 || m_size.x <= 0 || m_size.y <= 0) {
                return;
            }

            // Number of points per corner
            unsigned int pointsPerCorner = m_pointCount / 4;

            // For each corner, compute the arc points
            for (unsigned int corner = 0; corner < 4; ++corner) {
                for (unsigned int i = 0; i < pointsPerCorner; ++i) {
                    // Angle for this point on the quarter circle
                    float t = static_cast<float>(i) / static_cast<float>(pointsPerCorner - 1);
                    float angle = t * (M_PI / 2.0f);

                    // Base offset for rounded corner
                    float offsetX = m_radius * std::cos(angle);
                    float offsetY = m_radius * std::sin(angle);

                    sf::Vector2f point;

                    switch (corner) {
                    case 0: // Top-left corner
                        point.x = m_radius - offsetX;
                        point.y = m_radius - offsetY;
                        break;

                    case 1: // Top-right corner
                        point.x = m_size.x - m_radius + offsetY;
                        point.y = m_radius - offsetX;
                        break;

                    case 2: // Bottom-right corner
                        point.x = m_size.x - m_radius + offsetX;
                        point.y = m_size.y - m_radius + offsetY;
                        break;

                    case 3: // Bottom-left corner
                        point.x = m_radius - offsetY;
                        point.y = m_size.y - m_radius + offsetX;
                        break;
                    }

                    m_points.push_back(point);
                }
            }
        }
    };

} // namespace KE