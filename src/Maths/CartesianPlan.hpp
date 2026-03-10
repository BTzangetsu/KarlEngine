#pragma once
#include "Curve.hpp"
#include <unordered_map>
#include <cfloat>

//karlEngine namespace.
namespace KE {

	class CartesianPlan : public sf::Drawable {

	private:
		// Position of the plan widget in the window (top-left corner, in pixels)
		sf::Vector2f position;

		// Size of the plan widget in the window (width and height, in pixels)
		sf::Vector2f size;

		// Internal cartesian view for navigating through the data
		// This view handles zoom and pan operations
		sf::View cartesianView;

		// Viewport defining which portion of the window this plan occupies (normalized 0-1)
		sf::FloatRect viewport;

		// Map of curves and their vertices (stored in cartesian coordinates)
		// The sf::View handles the transformation to screen coordinates
		std::unordered_map<KE::Curve*, std::vector<sf::Vertex>> curveVertices;

		// Background color of the plan
		sf::Color backgroundColor = sf::Color::White;

		// Axis color
		sf::Color axisColor = sf::Color::Black;

		// Border color and thickness
		sf::Color borderColor = sf::Color::Black;
		float borderThickness = 2.0f;
		bool showBorder = true;

		// Updates the viewport based on position and size
		void updateViewport(const sf::RenderTarget& target) {
			sf::Vector2u windowSize = target.getSize();
			viewport.left = position.x / windowSize.x;
			viewport.top = position.y / windowSize.y;
			viewport.width = size.x / windowSize.x;
			viewport.height = size.y / windowSize.y;

			cartesianView.setViewport(viewport);
		}

	protected:

		// Draw the Cartesian plan on a render target
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			// Save the original view
			sf::View originalView = target.getView();

			// Update viewport (in case window was resized)
			const_cast<CartesianPlan*>(this)->updateViewport(target);

			// STEP 1: Draw background in screen coordinates
			target.setView(target.getDefaultView());

			sf::RectangleShape background(size);
			background.setPosition(position);
			background.setFillColor(backgroundColor);
			target.draw(background, states);

			// Draw border if enabled
			if (showBorder) {
				sf::RectangleShape border(size);
				border.setPosition(position);
				border.setFillColor(sf::Color::Transparent);
				border.setOutlineColor(borderColor);
				border.setOutlineThickness(borderThickness);
				target.draw(border, states);
			}

			// STEP 2: Activate the cartesian view for this region
			target.setView(cartesianView);

			// Draw the cartesian axes
			sf::Vector2f viewSize = cartesianView.getSize();
			sf::Vector2f viewCenter = cartesianView.getCenter();

			float left = viewCenter.x - viewSize.x / 2.0f;
			float right = viewCenter.x + viewSize.x / 2.0f;
			float top = viewCenter.y - viewSize.y / 2.0f;
			float bottom = viewCenter.y + viewSize.y / 2.0f;

			// X axis (horizontal)
			sf::Vertex xAxis[] = {
				sf::Vertex(sf::Vector2f(left, 0), axisColor),
				sf::Vertex(sf::Vector2f(right, 0), axisColor)
			};

			// Y axis (vertical)
			sf::Vertex yAxis[] = {
				sf::Vertex(sf::Vector2f(0, top), axisColor),
				sf::Vertex(sf::Vector2f(0, bottom), axisColor)
			};

			target.draw(xAxis, 2, sf::Lines, states);
			target.draw(yAxis, 2, sf::Lines, states);

			// Draw curves (directly in cartesian coordinates - the view handles transformation)
			for (const auto& pair : curveVertices) {
				const std::vector<sf::Vertex>& vertices = pair.second;
				if (!vertices.empty()) {
					target.draw(&vertices[0], vertices.size(), sf::Quads, states);
				}
			}

			// Restore the original view
			target.setView(originalView);
		}

	public:

		// Constructor with position and size
		CartesianPlan(const sf::Vector2f& position, const sf::Vector2f& size)
			: position(position), size(size) {

			// Initialize the cartesian view
			// Default: shows from -10 to +10 on each axis
			cartesianView.setSize(20.0f, 20.0f);
			cartesianView.setCenter(0.0f, 0.0f);
		}

		// Default constructor
		CartesianPlan()
			: CartesianPlan(sf::Vector2f(0, 0), sf::Vector2f(800, 600)) {}

		// Destructor (default is fine - no manual memory management needed)
		~CartesianPlan() = default;

		// Disable copy (contains pointers to external curves)
		CartesianPlan(const CartesianPlan&) = delete;
		CartesianPlan& operator=(const CartesianPlan&) = delete;

		// Enable move
		CartesianPlan(CartesianPlan&&) = default;
		CartesianPlan& operator=(CartesianPlan&&) = default;

		// === Widget position and size management ===

		// Set the position of the plan widget in the window
		void setPosition(const sf::Vector2f& newPosition) {
			position = newPosition;
		}

		// Get the position of the plan widget
		const sf::Vector2f& getPosition() const {
			return position;
		}

		// Set the size of the plan widget in the window
		void setSize(const sf::Vector2f& newSize) {
			size = newSize;
		}

		// Get the size of the plan widget
		const sf::Vector2f& getSize() const {
			return size;
		}

		// Check if a screen position (in pixels) is inside this plan widget
		bool containsPoint(const sf::Vector2f& screenPos) const {
			return screenPos.x >= position.x &&
				screenPos.x <= position.x + size.x &&
				screenPos.y >= position.y &&
				screenPos.y <= position.y + size.y;
		}

		// === Cartesian view navigation ===

		// Zoom in or out (factor < 1.0 = zoom in, factor > 1.0 = zoom out)
		void zoom(float factor) {
			cartesianView.zoom(factor);
		}

		// Pan the view by an offset in cartesian coordinates
		void pan(const sf::Vector2f& offset) {
			cartesianView.move(offset);
		}

		// Set the center of the cartesian view
		void setViewCenter(const sf::Vector2f& center) {
			cartesianView.setCenter(center);
		}

		// Get the center of the cartesian view
		sf::Vector2f getViewCenter() const {
			return cartesianView.getCenter();
		}

		// Set the size of the cartesian view (visible range)
		void setViewSize(const sf::Vector2f& viewSize) {
			cartesianView.setSize(viewSize);
		}

		// Get the size of the cartesian view
		sf::Vector2f getViewSize() const {
			return cartesianView.getSize();
		}

		// Get the cartesian view
		const sf::View& getCartesianView() const {
			return cartesianView;
		}

		// Reset the view to default (shows -10 to +10 on both axes)
		void resetView() {
			cartesianView.setSize(20.0f, 20.0f);
			cartesianView.setCenter(0.0f, 0.0f);
		}

		// Fit the view to show all curves with optional margin
		void fitToContent(float margin = 0.1f) {
			if (curveVertices.empty()) return;

			float minX = FLT_MAX, maxX = -FLT_MAX;
			float minY = FLT_MAX, maxY = -FLT_MAX;

			// Find bounds of all curves
			for (const auto& pair : curveVertices) {
				const auto& points = pair.first->getComputedPoints();
				for (const auto& point : points) {
					minX = std::min(minX, point.x);
					maxX = std::max(maxX, point.x);
					minY = std::min(minY, point.y);
					maxY = std::max(maxY, point.y);
				}
			}

			// Add margin
			float width = (maxX - minX) * (1.0f + margin);
			float height = (maxY - minY) * (1.0f + margin);
			float centerX = (minX + maxX) / 2.0f;
			float centerY = (minY + maxY) / 2.0f;

			// Ensure minimum size to avoid division by zero
			if (width < 0.1f) width = 20.0f;
			if (height < 0.1f) height = 20.0f;

			cartesianView.setCenter(centerX, centerY);
			cartesianView.setSize(width, height);
		}

		// === Coordinate conversion ===

		// Convert screen position (in pixels) to cartesian coordinates
		sf::Vector2f screenToCartesian(const sf::Vector2i& screenPos,
			const sf::RenderWindow& window) const {
			return window.mapPixelToCoords(screenPos, cartesianView);
		}

		// Convert cartesian coordinates to screen position (in pixels)
		sf::Vector2i cartesianToScreen(const sf::Vector2f& cartesianPos,
			const sf::RenderWindow& window) const {
			return window.mapCoordsToPixel(cartesianPos, cartesianView);
		}

		// === Curve management with Observer pattern ===

		// Add a curve to the cartesian plan
		// Note: The CartesianPlan does NOT own the curve - the caller is responsible
		// for keeping the curve alive as long as it's in the plan
		void addCurve(KE::Curve* curve) {
			if (!curve) {
				sf::err() << "Error: Cannot add null curve to CartesianPlan." << std::endl;
				return;
			}

			// Store the vertices (in cartesian coordinates directly)
			const std::vector<sf::Vertex>* src = curve->getCurveVertices();
			curveVertices[curve] = std::vector<sf::Vertex>(src->begin(), src->end());

			// Observer pattern: register this plan as an observer
			// Uses 'this' as unique identifier
			curve->addObserver(this, [this](Curve* modifiedCurve) {
				this->onCurveModified(modifiedCurve);
				});
		}

		// Remove a curve from the cartesian plan
		void removeCurve(KE::Curve* curve) {
			auto it = curveVertices.find(curve);
			if (it != curveVertices.end()) {
				curve->removeObserver(this);
				curveVertices.erase(it);
			}
		}

		// Called when a curve notifies that it has been modified
		void onCurveModified(KE::Curve* curve) {
			// Update the vertices for this curve
			const std::vector<sf::Vertex>* src = curve->getCurveVertices();
			curveVertices[curve] = std::vector<sf::Vertex>(src->begin(), src->end());
		}

		// Check if a curve is in the plan
		bool hasCurve(KE::Curve* curve) const {
			return curveVertices.find(curve) != curveVertices.end();
		}

		// Get number of curves in the plan
		size_t getCurveCount() const {
			return curveVertices.size();
		}

		// Clear all curves
		void clearCurves() {
			// Clear all observer callbacks
			for (auto& pair : curveVertices) {
				pair.first->removeObserver(this);
			}
			curveVertices.clear();
		}

		// === Appearance settings ===

		// Set background color
		void setBackgroundColor(const sf::Color& color) {
			backgroundColor = color;
		}

		// Get background color
		const sf::Color& getBackgroundColor() const {
			return backgroundColor;
		}

		// Set axis color
		void setAxisColor(const sf::Color& color) {
			axisColor = color;
		}

		// Get axis color
		const sf::Color& getAxisColor() const {
			return axisColor;
		}

		// Set border color
		void setBorderColor(const sf::Color& color) {
			borderColor = color;
		}

		// Get border color
		const sf::Color& getBorderColor() const {
			return borderColor;
		}

		// Set border thickness
		void setBorderThickness(float thickness) {
			borderThickness = thickness;
		}

		// Get border thickness
		float getBorderThickness() const {
			return borderThickness;
		}

		// Enable or disable border
		void setShowBorder(bool show) {
			showBorder = show;
		}

		// Check if border is shown
		bool isShowingBorder() const {
			return showBorder;
		}
	};
}