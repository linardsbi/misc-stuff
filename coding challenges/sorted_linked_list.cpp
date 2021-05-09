#include <cassert>
#include <memory>

template<typename NodeType>
class Node {
public:
    explicit Node(const NodeType &value)
            : value(std::move(value)) {}

    std::unique_ptr<Node> next{nullptr};
    NodeType value{};
};

template<typename NodeType>
class LinkedList {
    typedef bool (*compareFunc)(const NodeType &lhs, const NodeType &rhs);
    typedef bool (*compareUniqueFunc)(const NodeType &node);
public:

    LinkedList() = default;

    LinkedList(LinkedList<NodeType> &&list) noexcept {
        firstNode = std::move(list.firstNode);
        size = list.length();
        head = list.end();
        isSorted = list.isSorted;
    }

    LinkedList &operator=(LinkedList &&list) noexcept {
        if (this != &list) {
            firstNode = std::move(list.firstNode);
            size = list.length();
            head = list.end();
            isSorted = list.isSorted;
        }

        return *this;
    }

    std::size_t length() const {
        return this->size;
    }

    void push(NodeType value) {
        if (size == 0) {
            firstNode.reset(new Node<NodeType>(value));
            head = firstNode.get();
        } else {
            head->next.reset(new Node<NodeType>(value));
            head = head->next.get();
        }

        ++size;
        isSorted = false;
    }

    bool checkIsUnique(compareUniqueFunc func) {
        auto current = firstNode.get();
        while (current) {
            if (func(current->value)) {
                return false;
            }
            current = current->next.get();
        }
        return true;
    }

    NodeType pop(const bool front = false) {
        auto temp = this->firstNode.get();
        auto returnVal = temp->value;

        if (this->size < 1 || this->firstNode == nullptr) assert(false && "Tried to pop from empty list!");

        if (size == 1) {
            firstNode = nullptr;
            head = nullptr;
        } else if (front) {
            returnVal = firstNode->value;
            firstNode = std::move(firstNode->next);
        } else {
            while (true) {
                if (temp->next.get() == head) {
                    this->head = temp;
                    this->head->next = nullptr;
                    break;
                }

                temp = temp->next.get();
            }
            returnVal = temp->value;
        }

        --size;
        return returnVal;
    }

    Node<NodeType> *begin() const {
        return firstNode.get();
    }

    Node<NodeType> *end() const {
        return head;
    }

    void sort(compareFunc t_cmp) {
        if (isSorted && t_cmp == cmp) {
            return;
        }
        cmp = t_cmp;
        *this = this->mergeSort(*this);
        isSorted = true;
    }

private:
    std::unique_ptr<Node<NodeType>> firstNode = nullptr;
    Node<NodeType> *head = nullptr;
    compareFunc cmp = nullptr;
    std::size_t size = 0;
    bool isSorted = false;

    LinkedList mergeSort(LinkedList &list) {

        if (list.length() <= 1) return std::move(list);

        LinkedList left;
        LinkedList right;

        unsigned i = 0;

        for (auto n = list.begin(); n != nullptr; n = n->next.get()) {
            if (i < (list.length() / 2)) {
                left.push(n->value);
            } else {
                right.push(n->value);
            }

            i++;
        }

        left = this->mergeSort(left);
        right = this->mergeSort(right);

        return this->merge(left, right);
    }

    LinkedList merge(LinkedList &left, LinkedList &right) {
        LinkedList result;

        while (left.length() > 0 && right.length() > 0) {
            if (cmp(left.begin()->value, right.begin()->value)) {
                result.push(left.begin()->value);
                left.pop(true);
            } else {
                result.push(right.begin()->value);
                right.pop(true);
            }
        }

        while (left.length() > 0) {
            result.push(left.begin()->value);
            left.pop(true);
        }

        while (right.length() > 0) {
            result.push(right.begin()->value);
            right.pop(true);
        }

        return result;
    }
};
