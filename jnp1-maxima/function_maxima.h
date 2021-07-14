#ifndef FUNCTION_MAXIMA_H
#define FUNCTION_MAXIMA_H

#include <set>
#include <map>

class InvalidArg : public std::exception {
public:
    const char *what() const noexcept override {
        return "invalid argument value";
    }
};

template<typename A, typename V>
class FunctionMaxima {
public:
    class point_type {
    private:
        friend class FunctionMaxima<A, V>;

        std::pair<A, V> _point;

        explicit point_type(const A &a, const V &v) noexcept : _point(std::make_pair(a, v)) {}

    public:
        point_type(const point_type &point) = default;

        point_type &operator=(const point_type &point) noexcept {
            _point = point._point;
            return *this;
        }

        A const &arg() const noexcept {
            return _point.first;
        }

        V const &value() const noexcept {
            return _point.second;
        }
    };

private:
    struct function_cmp {
        bool operator()(const point_type &a, const point_type &b) const {
            return a.arg() < b.arg();
        }
    };

    struct maxima_cmp {
        bool operator()(const point_type &a, const point_type &b) const {
            if (!(a.value() < b.value()) && !(b.value() < a.value())) {
                return a.arg() < b.arg();
            } else {
                return b.value() < a.value();
            }
        }
    };

    std::set<point_type, function_cmp> _function;
    std::set<point_type, maxima_cmp> _maxima;

    bool add_max(const typename std::set<point_type, function_cmp>::iterator &it) {
        auto it_prev = it;
        it_prev--;
        auto it_next = it;
        it_next++;
        if ((it == _function.begin() || !(it->value() < it_prev->value())) &&
            (it_next == _function.end() || !(it->value() < it_next->value()))) {
            try {
                _maxima.insert(point_type(it->arg(), it->value()));
            } catch (...) {
                _maxima.erase(point_type(it->arg(), it->value()));
                throw;
            }
            return true;
        } else {
            return false;
        }
    }

    void rollback_set_value(A const &a, V const &v, V const &prev_v, size_t guard_fun, size_t guard_max,
                            const typename std::set<point_type, function_cmp>::iterator &it_prev,
                            const typename std::set<point_type, function_cmp>::iterator &it_next,
                            size_t s_prev, size_t s_next) {
        if (s_prev > 0) {
            _maxima.insert(point_type(it_prev->arg(), it_prev->value()));
        }
        if (s_next > 0) {
            _maxima.insert(point_type(it_next->arg(), it_next->value()));
        }
        _function.erase(point_type(a, v));
        if (guard_fun > 0) {
            _function.insert(point_type(a, prev_v));
        }
        if (guard_max > 0) {
            _maxima.insert(point_type(a, prev_v));
        }
        throw;
    }

    void rollback_erase(A const &a, V const &v,
                        const typename std::set<point_type, function_cmp>::iterator &it_prev,
                        const typename std::set<point_type, function_cmp>::iterator &it_next,
                        size_t s_prev, size_t s_next, size_t s_max, size_t s_fun) {
        if (s_prev > 0) {
            _maxima.insert(point_type(it_prev->arg(), it_prev->value()));
        }
        if (s_next > 0) {
            _maxima.insert(point_type(it_next->arg(), it_next->value()));
        }
        if (s_max > 0) {
            _maxima.insert(point_type(a, v));
        }
        if (s_fun > 0) {
            _function.insert(point_type(a, v));
        }
        throw;
    }

public:
    FunctionMaxima() noexcept = default;

    FunctionMaxima(const FunctionMaxima<A, V> &function) noexcept :
        _function(function._function),
        _maxima(function._maxima) {}

    FunctionMaxima<A, V> &operator=(const FunctionMaxima<A, V> &function) noexcept {
        _function = function._function;
        _maxima = function._maxima;
        return *this;
    }

    V const& value_at(A const &a) const {
        if (_function.empty()) {
            throw InvalidArg();
        }
        auto it = _function.find(point_type(a, _function.begin()->value()));
        if (it == _function.end()) {
            throw InvalidArg();
        }
        return it->value();
    }

    void set_value(A const &a, V const &v) {
        V prev_v = _function.find(point_type(a, _function.begin()->value()))->value();
        size_t s_max = 0;
        size_t s_fun = 0;
        try {
            s_max = _maxima.erase(point_type(a, prev_v));
            s_fun = _function.erase(point_type(a, prev_v));
            _function.insert(point_type(a, v));
        } catch (...) {
            if (s_max > 0) {
                _maxima.insert(point_type(a, prev_v));
            }
            if (s_fun > 0) {
                _function.insert(point_type(a, prev_v));
            }
            throw;
        }
        auto it = _function.find(point_type(a, v));
        auto it_prev = it;
        it_prev--;
        auto it_next = it;
        it_next++;
        size_t s_prev = 0;
        size_t s_next = 0;
        try {
            if (add_max(it)) {
                if (it != _function.begin() && it_prev->value() < it->value()) {
                    s_prev = _maxima.erase(point_type(it_prev->arg(), it_prev->value()));
                }
                if (it_next != _function.end() && it_next->value() < it->value()) {
                    s_next = _maxima.erase(point_type(it_next->arg(), it_next->value()));
                }
            } else {
                if (it != _function.begin()) {
                    add_max(it_prev);
                }
                if (it_next != _function.end()) {
                    add_max(it_next);
                }
            }
        } catch (...) {
            rollback_set_value(a, v, prev_v, s_fun, s_max, it_prev, it_next, s_prev, s_next);
        }
    }

    void erase(A const &a) {
        typename std::set<point_type, function_cmp>::iterator it;
        try {
            it = _function.find(point_type(a, value_at(a)));
        } catch (InvalidArg &e) {
            return;
        }
        V v = it->value();
        auto it_prev = it;
        it_prev--;
        auto it_next = it;
        it_next++;
        size_t s_prev, s_next, s_max, s_fun;
        s_prev = s_next = s_max = s_fun = 0;
        bool flag2 = false;
        try {
            if (it != _function.begin() && it_prev->value() < it->value()) {
                s_prev = _maxima.erase(point_type(it_prev->arg(), it_prev->value()));
            }
            if (it_next != _function.end() && it_next->value() < it->value()) {
                s_next = _maxima.erase(point_type(it_next->arg(), it_next->value()));
            }
            s_max = _maxima.erase(point_type(a, it->value()));
            bool flag = it != _function.begin();
            s_fun = _function.erase(point_type(a, value_at(a)));
            if (flag) {
                flag2 = add_max(it_prev);
            }
            if (it_next != _function.end()) {
                add_max(it_next);
            }
        } catch (...) {
            rollback_erase(a, v, it_prev, it_next, s_prev, s_next, s_max, s_fun);
        }
    }

    using iterator = typename std::set<point_type, function_cmp>::iterator;

    iterator begin() const noexcept {
        return _function.begin();
    }

    iterator end() const noexcept {
        return _function.end();
    }

    iterator find(A const& a) const noexcept {
        try {
            return _function.find(point_type(a, value_at(a)));
        } catch (InvalidArg &e) {
            return end();
        }
    }

    using mx_iterator = typename std::set<point_type, maxima_cmp>::iterator;

    mx_iterator mx_begin() const noexcept {
        return _maxima.begin();
    }

    mx_iterator mx_end() const noexcept {
        return _maxima.end();
    }

    using size_type = size_t;

    size_type size() const noexcept {
        return _function.size();
    }
};

#endif // FUNCTION_MAXIMA_H