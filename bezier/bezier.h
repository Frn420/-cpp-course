#ifndef BEZIER_H
#define BEZIER_H

#include <cmath>
#include <functional>
#include <iostream>
#include <utility>

namespace bezier {
    namespace types {
        using real_t = double;
        using node_index_t = size_t;

        struct point_2d {
            const real_t X;
            const real_t Y;

            point_2d(real_t x, real_t y) : X(x), Y(y) {}

            point_2d operator+(point_2d point) const {
                return point_2d(X + point.X, Y + point.Y);
            }

            point_2d operator*(real_t scalar) const {
                return point_2d(X * scalar, Y * scalar);
            }

            bool operator==(point_2d point) const {
                return X == point.X && Y == point.Y;
            }

            friend std::ostream &operator<<(std::ostream &s, point_2d point) {
                return s << "(" << point.X << ", " << point.Y << ")";
            }
        };

        point_2d operator*(real_t scalar, point_2d point) {
            return point * scalar;
        }

        namespace {
            using func_t = std::function<point_2d(node_index_t)>;
            using degree_t = unsigned int;
            using seg_index_t = size_t;
            using res_t = size_t;
            using print_t = char;
        }
    }

    namespace constants {
        const types::node_index_t NUM_OF_CUBIC_BEZIER_NODES = 4;
        const types::real_t ARC = 4.0 * (std::sqrt(2.0) - 1.0) / 3.0;
        namespace {
            const types::real_t RADIAN = 180.0;
            const types::real_t PRECISION = 1.2;
        }
    }

    types::func_t Cup() {
        return [](types::node_index_t index) -> types::point_2d {
            if (index == 0) {
                return types::point_2d(-1.0, 1.0);
            } else if (index == 1) {
                return types::point_2d(-1.0, -1.0);
            } else if (index == 2) {
                return types::point_2d(1.0, -1.0);
            } else if (index == 3) {
                return types::point_2d(1.0, 1.0);
            } else {
                throw std::out_of_range("a curve node index is out of range");
            }
        };
    }

    types::func_t Cap() {
        return [](types::node_index_t index) -> types::point_2d {
            if (index == 0) {
                return types::point_2d(-1.0, -1.0);
            } else if (index == 1) {
                return types::point_2d(-1.0, 1.0);
            } else if (index == 2) {
                return types::point_2d(1.0, 1.0);
            } else if (index == 3) {
                return types::point_2d(1.0, -1.0);
            } else {
                throw std::out_of_range("a curve node index is out of range");
            }
        };
    }

    types::func_t ConvexArc() {
        return [](types::node_index_t index) -> types::point_2d {
            if (index == 0) {
                return types::point_2d(0.0, 1.0);
            } else if (index == 1) {
                return types::point_2d(constants::ARC, 1.0);
            } else if (index == 2) {
                return types::point_2d(1.0, constants::ARC);
            } else if (index == 3) {
                return types::point_2d(1.0, 0.0);
            } else {
                throw std::out_of_range("a curve node index is out of range");
            }
        };
    }

    types::func_t ConcaveArc() {
        return [](types::node_index_t index) -> types::point_2d {
            if (index == 0) {
                return types::point_2d(0.0, 1.0);
            } else if (index == 1) {
                return types::point_2d(0.0, 1.0 - constants::ARC);
            } else if (index == 2) {
                return types::point_2d(1.0 - constants::ARC, 0.0);
            } else if (index == 3) {
                return types::point_2d(1.0, 0.0);
            } else {
                throw std::out_of_range("a curve node index is out of range");
            }
        };
    }

    types::func_t LineSegment(types::point_2d p, types::point_2d q) {
        return [p, q](types::node_index_t index) -> types::point_2d {
            if (index == 0 || index == 1) {
                return p;
            } else if (index == 2 || index == 3) {
                return q;
            } else {
                throw std::out_of_range("a curve node index is out of range");
            }
        };
    }

    types::func_t MovePoint(const types::func_t &f, types::node_index_t i, types::real_t x, types::real_t y) {
        return [f, i, x, y](types::node_index_t index) -> types::point_2d {
            if (index == i) {
                return f(index) + types::point_2d(x, y);
            } else {
                return f(index);
            }
        };
    }

    types::func_t Rotate(const types::func_t &f, types::degree_t a) {
        return [f, a](types::node_index_t index) -> types::point_2d {
            return types::point_2d(f(index).X * std::cos(a * M_PI / constants::RADIAN) -
                                   f(index).Y * std::sin(a * M_PI / constants::RADIAN),
                                   f(index).X * std::sin(a * M_PI / constants::RADIAN) +
                                   f(index).Y * std::cos(a * M_PI / constants::RADIAN));
        };
    }

    types::func_t Scale(const types::func_t &f, types::real_t x, types::real_t y) {
        return [f, x, y](types::node_index_t index) -> types::point_2d {
            return types::point_2d(f(index).X * x, f(index).Y * y);
        };
    }

    types::func_t Translate(const types::func_t &f, types::real_t x, types::real_t y) {
        return [f, x, y](types::node_index_t index) -> types::point_2d {
            return f(index) + types::point_2d(x, y);
        };
    }

    types::func_t Concatenate(const types::func_t &f1, const types::func_t &f2) {
        types::node_index_t f1_size = constants::NUM_OF_CUBIC_BEZIER_NODES;
        while (true) {
            try {
                f1(f1_size);
                f1_size += constants::NUM_OF_CUBIC_BEZIER_NODES;
            } catch (std::out_of_range &e) {
                break;
            }
        }
        return [f1, f2, f1_size](types::node_index_t index) -> types::point_2d {
            if (index < f1_size) {
                return f1(index);
            } else {
                return f2(index - f1_size);
            }
        };
    }

    template<typename T1, typename T2, typename... Args>
    types::func_t Concatenate(const T1 &f1, const T2 &f2, const Args &... args) {
        return Concatenate(Concatenate(f1, f2), args...);
    }

    class P3CurvePlotter {
    private:
        types::func_t func;
        types::seg_index_t seg;
        types::res_t res;

        static types::real_t distance(types::point_2d p1, types::point_2d p2) {
            types::real_t x = p1.X - p2.X;
            types::real_t y = p1.Y - p2.Y;
            return std::sqrt(x * x + y * y);
        }

        std::vector<types::point_2d> points() const {
            std::vector<types::point_2d> points;
            for (types::node_index_t i = 0; i < seg; i++) {
                types::node_index_t mult = i * constants::NUM_OF_CUBIC_BEZIER_NODES;
                types::real_t p0_p1 = distance(func(mult), func(mult + 1));
                types::real_t p1_p2 = distance(func(mult + 1), func(mult + 2));
                types::real_t p2_p3 = distance(func(mult + 2), func(mult + 3));
                types::real_t pix = (p0_p1 + p1_p2 + p2_p3) * res / 2.0;
                for (int j = 0; j < pix; j++) {
                    points.push_back((*this)(func, j / pix, i + 1));
                }
                points.push_back((*this)(func, 1, 1));
            }
            return points;
        }

    public:
        explicit P3CurvePlotter(types::func_t function) :
            func(std::move(function)),
            seg(1),
            res(80) {}

        P3CurvePlotter(types::func_t function, types::seg_index_t segments) :
            func(std::move(function)),
            seg(segments),
            res(80) {}

        P3CurvePlotter(types::func_t function, types::seg_index_t segments, types::res_t resolution) :
            func(std::move(function)),
            seg(segments),
            res(resolution) {}

        void Print() const {
            Print(std::cout, '*', ' ');
        }

        void Print(std::ostream &s) const {
            Print(s, '*', ' ');
        }

        void Print(std::ostream &s, types::print_t fb) const {
            Print(s, fb, ' ');
        }

        void Print(std::ostream &s, types::print_t fb, types::print_t bg) const {
            std::vector<types::point_2d> pts = points();
            types::real_t max_diff = constants::PRECISION / res;
            for (types::res_t i = res; i > 0; i--) {
                for (types::res_t j = 0; j < res; j++) {
                    bool check = false;
                    types::point_2d point(-1.0 + 2.0 * j / ((types::real_t)res - 1),
                                          -1.0 + 2.0 * (i - 1.0) / ((types::real_t)res - 1));
                    for (auto p : pts) {
                        if (distance(p, point) < max_diff) {
                            check = true;
                            break;
                        }
                    }
                    if (check) {
                        s << fb;
                    } else {
                        s << bg;
                    }
                }
                s << "\n";
            }
        }

        types::point_2d operator()(const types::func_t &function, types::real_t t, types::seg_index_t segment) const {
            types::node_index_t start = (segment - 1) * constants::NUM_OF_CUBIC_BEZIER_NODES;
            types::point_2d b0 = (1 - t) * function(start) + t * function(start + 1);
            types::point_2d b1 = (1 - t) * function(start + 1) + t * function(start + 2);
            types::point_2d b2 = (1 - t) * function(start + 2) + t * function(start + 3);
            types::point_2d new_b0 = (1 - t) * b0 + t * b1;
            types::point_2d new_b1 = (1 - t) * b1 + t * b2;
            return (1 - t) * new_b0 + t * new_b1;
        }
    };
}

#endif // BEZIER_H
