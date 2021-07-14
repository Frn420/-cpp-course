#include "geometry.h"

bool Position::operator==(const Position &secondPosition) const
{
    return _x == secondPosition._x && _y == secondPosition._y;
}

Position &Position::operator+=(const Vector &vector)
{
    _x += vector.x();
    _y += vector.y();
    return *this;
}

Position Position::reflection() const
{
    return Position(_y, _x);
}

const Position &Position::origin()
{
    static Position zero(0, 0);
    return zero;
}

bool Vector::operator==(const Vector &secondVector) const
{
    return _x == secondVector._x && _y == secondVector._y;
}

Vector &Vector::operator+=(const Vector &vector)
{
    _x += vector._x;
    _y += vector._y;
    return *this;
}

Vector Vector::reflection() const
{
    return Vector(_y, _x);
}

Vector::Vector(Position pos) : _x(pos.x()), _y(pos.y()) {}

bool Rectangle::operator==(const Rectangle &secondRectangle) const
{
    return _width == secondRectangle._width &&
           _height == secondRectangle._height &&
           _pos == secondRectangle._pos;
}

Rectangle &Rectangle::operator+=(const Vector &vector)
{
    _pos += vector;
    return *this;
}

Rectangle Rectangle::reflection() const
{
    return Rectangle(_height, _width, _pos.reflection());
}

Rectangle &Rectangles::operator[](size_t i)
{
    return _rectangles[i];
}

const Rectangle &Rectangles::operator[](size_t i) const
{
    return _rectangles[i];
}

bool Rectangles::operator==(const Rectangles &rectangles)
{
    return _rectangles == rectangles._rectangles;
}

Rectangles &Rectangles::operator+=(const Vector &vector)
{
    size_t size = _rectangles.size();
    for (size_t i = 0; i < size; i++)
        _rectangles[i] += vector;

    return *this;
}

Position operator+(const Position &position, const Vector &vector)
{
    return Position(position) += vector;
}

Position operator+(const Vector &vector, const Position &position)
{
    return position + vector;
}

Vector operator+(const Vector &firstVector, const Vector &secondVector)
{
    return Vector(firstVector) += secondVector;
}

Rectangle operator+(const Rectangle &rectangle, const Vector &vector)
{
    return Rectangle(rectangle) += vector;
}

Rectangle operator+(const Vector &vector, const Rectangle &rectangle)
{
    return rectangle + vector;
}

Rectangles operator+(const Rectangles &rectangles, const Vector &vector)
{
    return Rectangles(rectangles) += vector;
}

Rectangles operator+(const Vector &vector, const Rectangles &rectangles)
{
    return rectangles + vector;
}

Rectangles operator+(Rectangles &&rectangles, const Vector &vector)
{
    return std::move(rectangles += vector);
}

Rectangles operator+(const Vector &vector, Rectangles &&rectangles)
{
    return std::move(rectangles) + vector;
}

Rectangle merge_horizontally(const Rectangle &rec1, const Rectangle &rec2)
{
    assert(rec1.width() == rec2.width() && rec1.pos().x() == rec2.pos().x() &&
           rec1.pos().y() + rec1.height() == rec2.pos().y());
    return Rectangle(rec1.width(), abs(rec2.height() + rec1.height()), rec1.pos());
}

Rectangle merge_vertically(const Rectangle &rec1, const Rectangle &rec2)
{
    assert(rec1.height() == rec2.height() && rec1.pos().y() == rec2.pos().y() &&
           rec1.pos().x() + rec1.width() == rec2.pos().x());
    return Rectangle(abs(rec2.width() + rec1.width()), rec1.height(), rec1.pos());
}

namespace
{
    bool merge_one(Rectangle &rec1, const Rectangle &rec2)
    {
        if (rec1.pos().x() == rec2.pos().x())
        {
            rec1 = merge_horizontally(rec1, rec2);
            return true;
        }
        else if (rec1.pos().y() == rec2.pos().y())
        {
            rec1 = merge_vertically(rec1, rec2);
            return true;
        }
        return false;
    }
} // namespace

Rectangle merge_all(Rectangles rectangles)
{
    assert(rectangles.size());
    Rectangle final = rectangles[0];
    for (size_t i = 1; i < rectangles.size(); i++)
    {
        bool merge_done=merge_one(final, rectangles[i]);
        assert(merge_done);
    }
    return final;
}