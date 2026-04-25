
#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <memory>
#include <variant>

class pylist {
private:
    struct ListNode {
        std::vector<std::variant<int, std::shared_ptr<pylist>>> data;
    };
    
    std::shared_ptr<ListNode> node;
    
public:
    // Forward declare ListElement
    class ListElement;
    
    pylist() : node(std::make_shared<ListNode>()) {}
    
    // Copy constructor - shares the same underlying data
    pylist(const pylist& other) : node(other.node) {}
    
    // Assignment operator - shares the same underlying data
    pylist& operator=(const pylist& other) {
        if (this != &other) {
            node = other.node;
        }
        return *this;
    }
    
    void append(int x) {
        node->data.push_back(x);
    }
    
    void append(const pylist& x) {
        node->data.push_back(std::make_shared<pylist>(x));
    }
    
    void append(const ListElement& x);
    
    // Forward declare PopResult
    class PopResult;
    
    PopResult pop();
    
    // Helper class to handle both int and pylist access
    class ListElement {
    private:
        std::variant<int, std::shared_ptr<pylist>>& element;
        
        // Make pylist and PopResult friends so they can access element
        friend class pylist;
        friend class PopResult;
        
    public:
        ListElement(std::variant<int, std::shared_ptr<pylist>>& elem) : element(elem) {}
        
        // Conversion to int
        operator int() const {
            if (std::holds_alternative<int>(element)) {
                return std::get<int>(element);
            }
            throw std::bad_variant_access();
        }
        
        // Conversion to pylist
        operator pylist() const {
            if (std::holds_alternative<std::shared_ptr<pylist>>(element)) {
                return *std::get<std::shared_ptr<pylist>>(element);
            }
            throw std::bad_variant_access();
        }
        
        // Assignment from int
        ListElement& operator=(int value) {
            element = value;
            return *this;
        }
        
        // Assignment from pylist
        ListElement& operator=(const pylist& value) {
            element = std::make_shared<pylist>(value);
            return *this;
        }
        
        // Assignment from ListElement
        ListElement& operator=(const ListElement& other) {
            if (std::holds_alternative<int>(other.element)) {
                element = std::get<int>(other.element);
            } else if (std::holds_alternative<std::shared_ptr<pylist>>(other.element)) {
                element = std::get<std::shared_ptr<pylist>>(other.element);
            }
            return *this;
        }
        
        // Assignment from PopResult
        ListElement& operator=(const PopResult& other) {
            if (std::holds_alternative<int>(other.value)) {
                element = std::get<int>(other.value);
            } else if (std::holds_alternative<std::shared_ptr<pylist>>(other.value)) {
                element = std::get<std::shared_ptr<pylist>>(other.value);
            }
            return *this;
        }
        
        // Pop method for nested access
        PopResult pop() {
            if (std::holds_alternative<std::shared_ptr<pylist>>(element)) {
                return std::get<std::shared_ptr<pylist>>(element)->pop();
            }
            throw std::bad_variant_access();
        }
        
        // Append method - delegates to the underlying pylist
        void append(int x);
        void append(const pylist& x);
        void append(const ListElement& x);
        
        // Access nested list
        ListElement operator[](size_t index);
        
        // Output stream operator for ListElement
        friend std::ostream& operator<<(std::ostream& os, const ListElement& elem) {
            if (std::holds_alternative<int>(elem.element)) {
                os << std::get<int>(elem.element);
            } else if (std::holds_alternative<std::shared_ptr<pylist>>(elem.element)) {
                os << *std::get<std::shared_ptr<pylist>>(elem.element);
            }
            return os;
        }
    };
    
    ListElement operator[](size_t i) {
        if (i >= node->data.size()) {
            throw std::out_of_range("index out of range");
        }
        return ListElement(node->data[i]);
    }
    
    // Class to handle pop() return values
    class PopResult {
    private:
        std::variant<int, std::shared_ptr<pylist>> value;
        
        // Make pylist a friend so it can access value
        friend class pylist;
        
    public:
        PopResult(const std::variant<int, std::shared_ptr<pylist>>& val) : value(val) {}
        
        // Conversion to int
        operator int() const {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            }
            throw std::bad_variant_access();
        }
        
        // Conversion to pylist
        operator pylist() const {
            if (std::holds_alternative<std::shared_ptr<pylist>>(value)) {
                return *std::get<std::shared_ptr<pylist>>(value);
            }
            throw std::bad_variant_access();
        }
        
        // Pop method for nested access
        PopResult pop() {
            if (std::holds_alternative<std::shared_ptr<pylist>>(value)) {
                return std::get<std::shared_ptr<pylist>>(value)->pop();
            }
            throw std::bad_variant_access();
        }
        
        // Access nested list
        ListElement operator[](size_t index) {
            if (std::holds_alternative<std::shared_ptr<pylist>>(value)) {
                return (*std::get<std::shared_ptr<pylist>>(value))[index];
            }
            throw std::bad_variant_access();
        }
        
        // Output stream operator for PopResult
        friend std::ostream& operator<<(std::ostream& os, const PopResult& result) {
            if (std::holds_alternative<int>(result.value)) {
                os << std::get<int>(result.value);
            } else if (std::holds_alternative<std::shared_ptr<pylist>>(result.value)) {
                os << *std::get<std::shared_ptr<pylist>>(result.value);
            }
            return os;
        }
    };
    
    friend std::ostream& operator<<(std::ostream& os, const pylist& ls) {
        os << "[";
        for (size_t i = 0; i < ls.node->data.size(); ++i) {
            if (i > 0) os << ", ";
            
            if (std::holds_alternative<int>(ls.node->data[i])) {
                os << std::get<int>(ls.node->data[i]);
            } else if (std::holds_alternative<std::shared_ptr<pylist>>(ls.node->data[i])) {
                os << *std::get<std::shared_ptr<pylist>>(ls.node->data[i]);
            }
        }
        os << "]";
        return os;
    }
};

// Define the methods outside the class to avoid circular dependencies
inline void pylist::append(const ListElement& x) {
    if (std::holds_alternative<int>(x.element)) {
        append(std::get<int>(x.element));
    } else if (std::holds_alternative<std::shared_ptr<pylist>>(x.element)) {
        append(*std::get<std::shared_ptr<pylist>>(x.element));
    }
}

inline void pylist::ListElement::append(int x) {
    if (std::holds_alternative<std::shared_ptr<pylist>>(element)) {
        std::get<std::shared_ptr<pylist>>(element)->append(x);
    } else {
        // Convert int to pylist and append
        pylist temp;
        temp.append(std::get<int>(element));
        temp.append(x);
        element = std::make_shared<pylist>(temp);
    }
}

inline void pylist::ListElement::append(const pylist& x) {
    if (std::holds_alternative<std::shared_ptr<pylist>>(element)) {
        std::get<std::shared_ptr<pylist>>(element)->append(x);
    } else {
        // Convert int to pylist and append
        pylist temp;
        temp.append(std::get<int>(element));
        temp.append(x);
        element = std::make_shared<pylist>(temp);
    }
}

inline void pylist::ListElement::append(const ListElement& x) {
    if (std::holds_alternative<int>(x.element)) {
        append(std::get<int>(x.element));
    } else if (std::holds_alternative<std::shared_ptr<pylist>>(x.element)) {
        append(*std::get<std::shared_ptr<pylist>>(x.element));
    }
}

inline pylist::ListElement pylist::ListElement::operator[](size_t index) {
    if (std::holds_alternative<std::shared_ptr<pylist>>(element)) {
        return (*std::get<std::shared_ptr<pylist>>(element))[index];
    }
    throw std::bad_variant_access();
}

inline pylist::PopResult pylist::pop() {
    if (node->data.empty()) {
        throw std::out_of_range("pop from empty list");
    }
    auto result = node->data.back();
    node->data.pop_back();
    return PopResult(result);
}

#endif //PYLIST_H
