#include "expression.hpp"
#include <sstream>
#include <memory>
#include <cmath>

using namespace std;

// Определение структуры Node
template <typename T>
struct Expression<T>::Node {
    enum class Type {
        CONSTANT,
        VARIABLE,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        POWER,
        SIN,
        COS,
        EXP,
        LOG,
        NEGATE
    };
    
    Type type;
    T value;
    string variable;
    shared_ptr<Node> left;
    shared_ptr<Node> right;
    
    Node(Type t, T val) : type(t), value(val), left(nullptr), right(nullptr) {}
    Node(Type t, string var) : type(t), variable(move(var)), left(nullptr), right(nullptr) {}
    Node(Type t, shared_ptr<Node> l, shared_ptr<Node> r) 
        : type(t), left(move(l)), right(move(r)) {}
    Node(Type t, shared_ptr<Node> l) 
        : type(t), left(move(l)), right(nullptr) {}
};

// Реализация методов Expression
template <typename T>
Expression<T>::Expression() : root(make_shared<Node>(Node::Type::CONSTANT, T(0))) {}

template <typename T>
Expression<T>::Expression(T value) : root(make_shared<Node>(Node::Type::CONSTANT, value)) {}

template <typename T>
Expression<T>::Expression(const string& variable) 
    : root(make_shared<Node>(Node::Type::VARIABLE, variable)) {}

template <typename T>
Expression<T>::Expression(shared_ptr<Node> node) : root(move(node)) {}

// Арифметические операции
template <typename T>
Expression<T> Expression<T>::operator+(const Expression& other) const {
    return Expression(make_shared<Node>(Node::Type::ADD, root, other.root));
}

template <typename T>
Expression<T> Expression<T>::operator-(const Expression& other) const {
    return Expression(make_shared<Node>(Node::Type::SUBTRACT, root, other.root));
}

template <typename T>
Expression<T> Expression<T>::operator*(const Expression& other) const {
    return Expression(make_shared<Node>(Node::Type::MULTIPLY, root, other.root));
}

template <typename T>
Expression<T> Expression<T>::operator/(const Expression& other) const {
    return Expression(make_shared<Node>(Node::Type::DIVIDE, root, other.root));
}

template <typename T>
Expression<T> Expression<T>::operator-() const {
    return Expression(make_shared<Node>(Node::Type::NEGATE, root));
}

// Математические функции
template <typename T>
Expression<T> Expression<T>::sin(const Expression& expr) {
    return Expression(make_shared<Node>(Node::Type::SIN, expr.root));
}

template <typename T>
Expression<T> Expression<T>::cos(const Expression& expr) {
    return Expression(make_shared<Node>(Node::Type::COS, expr.root));
}

template <typename T>
Expression<T> Expression<T>::exp(const Expression& expr) {
    return Expression(make_shared<Node>(Node::Type::EXP, expr.root));
}

template <typename T>
Expression<T> Expression<T>::log(const Expression& expr) {
    return Expression(make_shared<Node>(Node::Type::LOG, expr.root));
}

template <typename T>
Expression<T> Expression<T>::pow(const Expression& base, const Expression& exponent) {
    return Expression(make_shared<Node>(Node::Type::POWER, base.root, exponent.root));
}

// Вычисление выражения
template <typename T>
T Expression<T>::evaluate(const map<string, T>& variables) const {
    switch (root->type) {
        case Node::Type::CONSTANT:
            return root->value;
        case Node::Type::VARIABLE: {
            auto it = variables.find(root->variable);
            if (it != variables.end()) return it->second;
            throw runtime_error("Undefined variable: " + root->variable);
        }
        case Node::Type::ADD:
            return Expression(root->left).evaluate(variables) + 
                   Expression(root->right).evaluate(variables);
        case Node::Type::SUBTRACT:
            return Expression(root->left).evaluate(variables) - 
                   Expression(root->right).evaluate(variables);
        case Node::Type::MULTIPLY:
            return Expression(root->left).evaluate(variables) * 
                   Expression(root->right).evaluate(variables);
        case Node::Type::DIVIDE:
            return Expression(root->left).evaluate(variables) / 
                   Expression(root->right).evaluate(variables);
        case Node::Type::POWER:
            return std::pow(Expression(root->left).evaluate(variables),
                          Expression(root->right).evaluate(variables));
        case Node::Type::SIN:
            return std::sin(Expression(root->left).evaluate(variables));
        case Node::Type::COS:
            return std::cos(Expression(root->left).evaluate(variables));
        case Node::Type::EXP:
            return std::exp(Expression(root->left).evaluate(variables));
        case Node::Type::LOG:
            return std::log(Expression(root->left).evaluate(variables));
        case Node::Type::NEGATE:
            return -Expression(root->left).evaluate(variables);
    }
    return T(0);
}

// Вычисление производной
template <typename T>
Expression<T> Expression<T>::derivative(const string& variable) const {
    return Expression(derivative(root, variable));
}

template <typename T>
shared_ptr<typename Expression<T>::Node> Expression<T>::derivative(
    const shared_ptr<Node>& node, const string& variable) 
{
    switch (node->type) {
        case Node::Type::CONSTANT:
            return make_shared<Node>(Node::Type::CONSTANT, T(0));
        case Node::Type::VARIABLE:
            return make_shared<Node>(Node::Type::CONSTANT, 
                (node->variable == variable) ? T(1) : T(0));
        case Node::Type::ADD:
        case Node::Type::SUBTRACT:
            return make_shared<Node>(node->type, 
                derivative(node->left, variable),
                derivative(node->right, variable));
        case Node::Type::MULTIPLY: {
            // (uv)' = u'v + uv'
            auto u = node->left, v = node->right;
            auto du = derivative(u, variable), dv = derivative(v, variable);
            auto term1 = make_shared<Node>(Node::Type::MULTIPLY, du, v);
            auto term2 = make_shared<Node>(Node::Type::MULTIPLY, u, dv);
            return make_shared<Node>(Node::Type::ADD, term1, term2);
        }
        case Node::Type::DIVIDE: {
            // (u/v)' = (u'v - uv')/v^2
            auto u = node->left, v = node->right;
            auto du = derivative(u, variable), dv = derivative(v, variable);
            auto num1 = make_shared<Node>(Node::Type::MULTIPLY, du, v);
            auto num2 = make_shared<Node>(Node::Type::MULTIPLY, u, dv);
            auto numerator = make_shared<Node>(Node::Type::SUBTRACT, num1, num2);
            auto denominator = make_shared<Node>(Node::Type::POWER, v, 
                make_shared<Node>(Node::Type::CONSTANT, T(2)));
            return make_shared<Node>(Node::Type::DIVIDE, numerator, denominator);
        }
        case Node::Type::POWER: {
            // u^v: упрощаем если v - константа
            auto u = node->left, v = node->right;
            Expression<T> v_expr(v);
            if (v_expr.isConstant()) {
                // (u^n)' = n*u^(n-1)*u'
                T n = v_expr.evaluate();
                auto term1 = make_shared<Node>(Node::Type::CONSTANT, n);
                auto term2 = make_shared<Node>(Node::Type::POWER, u, 
                    make_shared<Node>(Node::Type::CONSTANT, n - T(1))); // Исправлено
                auto term3 = derivative(u, variable);
                auto part = make_shared<Node>(Node::Type::MULTIPLY, term1, term2);
                return make_shared<Node>(Node::Type::MULTIPLY, part, term3);
            }
            throw runtime_error("Derivative of non-constant exponents not implemented");
        }
        case Node::Type::SIN: {
            auto u = node->left;
            auto cos_u = make_shared<Node>(Node::Type::COS, u);
            auto du = derivative(u, variable);
            return make_shared<Node>(Node::Type::MULTIPLY, cos_u, du);
        }
        case Node::Type::COS: {
            auto u = node->left;
            auto sin_u = make_shared<Node>(Node::Type::SIN, u);
            auto neg_sin = make_shared<Node>(Node::Type::NEGATE, sin_u);
            auto du = derivative(u, variable);
            return make_shared<Node>(Node::Type::MULTIPLY, neg_sin, du);
        }
        case Node::Type::EXP: {
            auto u = node->left;
            auto exp_u = make_shared<Node>(Node::Type::EXP, u);
            auto du = derivative(u, variable);
            return make_shared<Node>(Node::Type::MULTIPLY, exp_u, du);
        }
        case Node::Type::LOG: {
            auto u = node->left;
            auto du = derivative(u, variable);
            return make_shared<Node>(Node::Type::DIVIDE, du, u);
        }
        case Node::Type::NEGATE: {
            auto u = node->left;
            auto du = derivative(u, variable);
            return make_shared<Node>(Node::Type::NEGATE, du);
        }
    }
    return make_shared<Node>(Node::Type::CONSTANT, T(0));
}

// Подстановка значения переменной
template <typename T>
Expression<T> Expression<T>::substitute(const string& variable, const Expression& value) const {
    if (root->type == Node::Type::VARIABLE && root->variable == variable) {
        return value;
    }
    
    shared_ptr<Node> newLeft = root->left ? 
        Expression(root->left).substitute(variable, value).root : nullptr;
    shared_ptr<Node> newRight = root->right ? 
        Expression(root->right).substitute(variable, value).root : nullptr;
    
    if (!newLeft && !newRight) return *this;
    
    return Expression(make_shared<Node>(root->type, 
        newLeft ? newLeft : root->left,
        newRight ? newRight : root->right));
}

// Преобразование в строку
template <typename T>
string Expression<T>::toString() const {
    switch (root->type) {
        case Node::Type::CONSTANT: {
            stringstream ss;
            ss << root->value;
            return ss.str();
        }
        case Node::Type::VARIABLE:
            return root->variable;
        case Node::Type::ADD:
            return "(" + Expression(root->left).toString() + " + " + 
                   Expression(root->right).toString() + ")";
        case Node::Type::SUBTRACT:
            return "(" + Expression(root->left).toString() + " - " + 
                   Expression(root->right).toString() + ")";
        case Node::Type::MULTIPLY:
            return "(" + Expression(root->left).toString() + " * " + 
                   Expression(root->right).toString() + ")";
        case Node::Type::DIVIDE:
            return "(" + Expression(root->left).toString() + " / " + 
                   Expression(root->right).toString() + ")";
        case Node::Type::POWER:
            return "pow(" + Expression(root->left).toString() + ", " + 
                   Expression(root->right).toString() + ")";
        case Node::Type::SIN:
            return "sin(" + Expression(root->left).toString() + ")";
        case Node::Type::COS:
            return "cos(" + Expression(root->left).toString() + ")";
        case Node::Type::EXP:
            return "exp(" + Expression(root->left).toString() + ")";
        case Node::Type::LOG:
            return "log(" + Expression(root->left).toString() + ")";
        case Node::Type::NEGATE:
            return "-(" + Expression(root->left).toString() + ")";
    }
    return "";
}

// Проверки
template <typename T>
bool Expression<T>::isConstant() const {
    switch (root->type) {
        case Node::Type::CONSTANT:
            return true;
        case Node::Type::VARIABLE:
            return false;
        case Node::Type::ADD:
        case Node::Type::SUBTRACT:
        case Node::Type::MULTIPLY:
        case Node::Type::DIVIDE:
        case Node::Type::POWER:
            return Expression(root->left).isConstant() && 
                   Expression(root->right).isConstant();
        case Node::Type::SIN:
        case Node::Type::COS:
        case Node::Type::EXP:
        case Node::Type::LOG:
        case Node::Type::NEGATE:
            return Expression(root->left).isConstant();
    }
    return false;
}

template <typename T>
bool Expression<T>::isVariable() const {
    return root->type == Node::Type::VARIABLE;
}

template <typename T>
bool Expression<T>::isVariable(const string& var) const {
    return root->type == Node::Type::VARIABLE && root->variable == var;
}

// Явное инстанцирование шаблонов
template class Expression<double>;
template class Expression<complex<double>>;
