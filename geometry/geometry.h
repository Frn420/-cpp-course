#ifndef GEOMETRY_GEOMETRY_H
#define GEOMETRY_GEOMETRY_H

#include <cassert>
#include <cinttypes>
#include <vector>

class Position;
class Vector
{
private:
    int32_t _x, _y;

public:
    Vector(int32_t x, int32_t y) : _x(x), _y(y) {}
    explicit Vector(Position pos);
    Vector(const Vector &secondVector) = default;

    int32_t x() const { return _x; }

    int32_t y() const { return _y; }

    Vector &operator=(const Vector &secondVector) = default;

    bool operator==(const Vector &secondVector) const;

    Vector &operator+=(const Vector &vector);

    Vector reflection() const;
};

class Position
{
private:
    int32_t _x, _y;

public:
    Position(int32_t x, int32_t y) : _x(x), _y(y) {}
    explicit Position(Vector vec) : _x(vec.x()), _y(vec.y()) {}
    Position(const Position &secondPosition) = default;

    int32_t x() const { return _x; }

    int32_t y() const { return _y; }

    Position &operator=(const Position &secondPosition) = default;

    bool operator==(const Position &secondPosition) const;

    Position &operator+=(const Vector &vector);

    Position reflection() const;

    static const Position &origin();
};

class Rectangle
{
private:
    int32_t _width, _height;
    Position _pos;

public:
    Rectangle(int32_t width, int32_t height, Position pos = Position::origin()) :
            _width(width),
            _height(height),
            _pos(pos)
    {
        assert(width > 0);
        assert(height > 0);
    }
    Rectangle(const Rectangle &secondRectangle) = default;

    int32_t width() const { return _width; }

    int32_t height() const { return _height; }

    Position pos() const { return _pos; }

    int32_t area() const { return _width * _height; }

    Rectangle &operator=(const Rectangle &secondRectangle) = default;

    bool operator==(const Rectangle &secondRectangle) const;

    Rectangle &operator+=(const Vector &vector);

    Rectangle reflection() const;
};

class Rectangles
{
private:
    std::vector<Rectangle> _rectangles;

public:
    Rectangles() = default;
    Rectangles(std::initializer_list<Rectangle> rectangles) : _rectangles(rectangles) {}

    size_t size() const { return _rectangles.size(); }

    Rectangle &operator[](size_t i);

    const Rectangle &operator[](size_t i) const;

    bool operator==(const Rectangles &rectangles);

    Rectangles &operator+=(const Vector &vector);
};

Position operator+(const Position &position, const Vector &vector);

Position operator+(const Vector &vector, const Position &position);

Vector operator+(const Vector &firstVector, const Vector &secondVector);

Rectangle operator+(const Rectangle &rectangle, const Vector &vector);

Rectangle operator+(const Vector &vector, const Rectangle &rectangle);

Rectangles operator+(const Rectangles &rectangles, const Vector &vector);

Rectangles operator+(const Vector &vector, const Rectangles &rectangles);

Rectangles operator+(Rectangles &&rectangles, const Vector &vector);

Rectangles operator+(const Vector &vector, Rectangles &&rectangles);

Rectangle merge_horizontally(const Rectangle &rec1, const Rectangle &rec2);

Rectangle merge_vertically(const Rectangle &rec1, const Rectangle &rec2);

Rectangle merge_all(Rectangles rectangles);

#endif //GEOMETRY_GEOMETRY_H