
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
    
    pylist pop() {
        if (node->data.empty()) {
            throw std::out_of_range("pop from empty list");
        }
        auto result = node->data.back();
        node->data.pop_back();
        
        if (std::holds_alternative<int>(result)) {
            pylist temp;
            temp.append(std::get<int>(result));
            return temp;
        } else {
            return *std::get<std::shared_ptr<pylist>>(result);
        }
    }
    
    // Helper class to handle both int and pylist access
    class ListElement {
    private:
        std::variant<int, std::shared_ptr<pylist>>& element;
        
        // Make pylist a friend so it can access element
        friend class pylist;
        
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

#endif //PYLIST_H
