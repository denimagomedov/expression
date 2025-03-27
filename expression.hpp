#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <memory>    // Для std::shared_ptr
#include <string>    // Для работы со строками
#include <map>       // Для std::map (хранение переменных)
#include <complex>   // Для поддержки комплексных чисел
#include <cmath>     // Математические функции
#include <stdexcept> // Исключения
#include <iostream>  // Для отладки и вывода

/**
 * @brief Шаблонный класс для символьных вычислений и дифференцирования
 * @tparam T Тип данных (double, complex<double> и т.д.)
 * 
 * Класс представляет математические выражения, которые могут содержать:
 * - Константы
 * - Переменные
 * - Арифметические операции
 * - Элементарные функции
 * Поддерживает вычисление значений, дифференцирование и подстановку значений.
 */
template <typename T>
class Expression {
public:
    // ==================== КОНСТРУКТОРЫ ====================
    
    /**
     * @brief Конструктор по умолчанию (создает выражение равное 0)
     */
    Expression();
    
    /**
     * @brief Конструктор из числового значения
     * @param value Числовое значение константы
     */
    explicit Expression(T value);
    
    /**
     * @brief Конструктор из переменной
     * @param variable Имя переменной (например, "x")
     */
    explicit Expression(const std::string& variable);
    
    // ==================== УПРАВЛЕНИЕ ПАМЯТЬЮ ====================
    
    // Правило пяти (по умолчанию)
    Expression(const Expression& other) = default;
    Expression(Expression&& other) noexcept = default;
    Expression& operator=(const Expression& other) = default;
    Expression& operator=(Expression&& other) noexcept = default;
    ~Expression() = default;
    
    // ==================== АРИФМЕТИЧЕСКИЕ ОПЕРАЦИИ ====================
    
    Expression operator+(const Expression& other) const; ///< Сложение выражений
    Expression operator-(const Expression& other) const; ///< Вычитание выражений
    Expression operator*(const Expression& other) const; ///< Умножение выражений
    Expression operator/(const Expression& other) const; ///< Деление выражений
    Expression operator-() const; ///< Унарный минус (отрицание)
    
    // ==================== МАТЕМАТИЧЕСКИЕ ФУНКЦИИ ====================
    
    static Expression sin(const Expression& expr);  ///< Синус выражения
    static Expression cos(const Expression& expr);  ///< Косинус выражения
    static Expression exp(const Expression& expr);  ///< Экспонента выражения
    static Expression log(const Expression& expr);  ///< Натуральный логарифм
    static Expression pow(const Expression& base, const Expression& exponent); ///< Возведение в степень
    
    // ==================== ОСНОВНЫЕ ОПЕРАЦИИ ====================
    
    /**
     * @brief Вычисление значения выражения
     * @param variables Словарь значений переменных
     * @return Результат вычисления
     */
    T evaluate(const std::map<std::string, T>& variables = {}) const;
    
    /**
     * @brief Вычисление производной по переменной
     * @param variable Имя переменной дифференцирования
     * @return Новое выражение - производная
     */
    Expression derivative(const std::string& variable) const;
    
    /**
     * @brief Подстановка значения переменной
     * @param variable Имя заменяемой переменной
     * @param value Выражение для подстановки
     * @return Новое выражение с подстановкой
     */
    Expression substitute(const std::string& variable, const Expression& value) const;
    
    // ==================== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ====================
    
    /**
     * @brief Преобразование выражения в строку
     * @return Строковое представление выражения
     */
    std::string toString() const;
    
    /**
     * @brief Проверка, является ли выражение константой
     */
    bool isConstant() const;
    
    /**
     * @brief Проверка, является ли выражение переменной
     */
    bool isVariable() const;
    
    /**
     * @brief Проверка, является ли выражение конкретной переменной
     * @param var Имя переменной для проверки
     */
    bool isVariable(const std::string& var) const;

private:
    // Внутренняя структура для представления узлов AST
    struct Node;
    
    // Корень дерева выражения
    std::shared_ptr<Node> root;
    
    /**
     * @brief Приватный конструктор из узла AST
     * @param node Узел для создания выражения
     */
    explicit Expression(std::shared_ptr<Node> node);
    
    // ==================== ВНУТРЕННИЕ МЕТОДЫ ====================
    
    /**
     * @brief Рекурсивное вычисление производной
     * @param node Текущий узел для дифференцирования
     * @param variable Имя переменной дифференцирования
     * @return Новый узел - производная
     */
    static std::shared_ptr<Node> derivative(const std::shared_ptr<Node>& node, 
                                          const std::string& variable);
    
    /**
     * @brief Упрощение выражения
     * @param node Узел для упрощения
     * @return Упрощенный узел
     */
    static std::shared_ptr<Node> simplify(const std::shared_ptr<Node>& node);
};

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

/**
 * @brief Создание выражения-синуса
 */
template <typename T>
Expression<T> sin(const Expression<T>& expr) {
    return Expression<T>::sin(expr);
}

/**
 * @brief Создание выражения-косинуса
 */
template <typename T>
Expression<T> cos(const Expression<T>& expr) {
    return Expression<T>::cos(expr);
}

/**
 * @brief Создание выражения-экспоненты
 */
template <typename T>
Expression<T> exp(const Expression<T>& expr) {
    return Expression<T>::exp(expr);
}

/**
 * @brief Создание выражения-логарифма
 */
template <typename T>
Expression<T> log(const Expression<T>& expr) {
    return Expression<T>::log(expr);
}

/**
 * @brief Возведение выражения в степень
 */
template <typename T>
Expression<T> pow(const Expression<T>& base, const Expression<T>& exponent) {
    return Expression<T>::pow(base, exponent);
}

/**
 * @brief Возведение выражения в числовую степень
 */
template <typename T>
Expression<T> pow(const Expression<T>& base, T exponent) {
    return Expression<T>::pow(base, Expression<T>(exponent));
}

#endif // EXPRESSION_HPP