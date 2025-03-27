#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <memory>
#include <string>
#include <map>
#include <complex>
#include <cmath>
#include <stdexcept>
#include <iostream>

template <typename T>
class Expression {
public:
    Expression();
    explicit Expression(T value);
    explicit Expression(const std::string& variable);
    
    Expression(const Expression& other) = default;
    Expression(Expression&& other) noexcept = default;
    Expression& operator=(const Expression& other) = default;
    Expression& operator=(Expression&& other) noexcept = default;
    ~Expression() = default;
    
    Expression operator+(const Expression& other) const;
    Expression operator-(const Expression& other) const;
    Expression operator*(const Expression& other) const;
    Expression operator/(const Expression& other) const;
    Expression operator-() const;
    
    static Expression sin(const Expression& expr);
    static Expression cos(const Expression& expr);
    static Expression exp(const Expression& expr);
    static Expression log(const Expression& expr);
    static Expression pow(const Expression& base, const Expression& exponent);
    
    T evaluate(const std::map<std::string, T>& variables = {}) const;
    Expression derivative(const std::string& variable) const;
    Expression substitute(const std::string& variable, const Expression& value) const;
    
    std::string toString() const;
    
    bool isConstant() const;
    bool isVariable() const;
    bool isVariable(const std::string& var) const;

private:
    struct Node;
    std::shared_ptr<Node> root;
    
    explicit Expression(std::shared_ptr<Node> node);
    
    static std::shared_ptr<Node> derivative(const std::shared_ptr<Node>& node, 
                                          const std::string& variable);
    static std::shared_ptr<Node> simplify(const std::shared_ptr<Node>& node);
};

template <typename T>
Expression<T> sin(const Expression<T>& expr) {
    return Expression<T>::sin(expr);
}

template <typename T>
Expression<T> cos(const Expression<T>& expr) {
    return Expression<T>::cos(expr);
}

template <typename T>
Expression<T> exp(const Expression<T>& expr) {
    return Expression<T>::exp(expr);
}

template <typename T>
Expression<T> log(const Expression<T>& expr) {
    return Expression<T>::log(expr);
}

template <typename T>
Expression<T> pow(const Expression<T>& base, const Expression<T>& exponent) {
    return Expression<T>::pow(base, exponent);
}

template <typename T>
Expression<T> pow(const Expression<T>& base, T exponent) {
    return Expression<T>::pow(base, Expression<T>(exponent));
}

#endif // EXPRESSION_HPP
