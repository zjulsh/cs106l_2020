/*
* Assignment 2: HashMap template implementation (STARTER CODE)
*
*      TODO: write a comment here.
*
*      You'll notice that the commenting provided is absolutely stunning.
*      It was really fun to read through the starter code, right?
*      Please emulate this commenting style in your code, so your grader
*      can have an equally pleasant time reading your code. :)
*/

#include "hashmap.h"
#include <algorithm>
#include <iostream>

/*
 * Notes: in the member initializer list (see the next function for an
 * explanation), we call another constructor. This is called a delegating constructor.
 * We simply call the second constructor below with default parameters.
 *
* Notes: here we use a delegating constructor. Here, we simply
* call the second constructor below, passing in a default bucket_count of 10.
*/
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : HashMap(kDefaultBuckets) { }

/*
* Notes: here we use a member initializer list to construct each of
* our members. Recall that a member initializer list is superior to
* assigning each member. Member initializer lists directly construct
* a member, while assigning results in a default construction of a member
* followed by reassignment. In fact, some types (references, uncopyable types)
* cannot be reassigned, so member initializer lists are necessary. The type of
* H here is likely a lambda function, which you've seen is non-copyable, so
* the code won't compile if you use direct assignment.
*/
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash) :
    _size(0),
    _hash_function(hash),
    _buckets_array(bucket_count) { }

/*
* Notes: The clear() call frees all the nodes. When the destructor
* calls free, it will later free the memory for the vector, so we
* don't have to worry about that here.
*/
template <typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
    clear();
}

/*
* Notes: what is inline? inline is a non-binding directive to the
* compiler to prefer inline substitution (copy the body of the function
* to the location of the function call), rather than actually make
* a function call. This is faster, particularly if the code is very short.
*/
template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const noexcept {
    return _size;
}

/*
 * Notes: prefer calling external member functions (size()) rather than
 * calling private functions or members whenever possible. This ensures
 * self-consistency between empty() and size().
 */
template <typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const noexcept {
    return size() == 0;
}

template <typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const noexcept {
    return static_cast<float>(size())/bucket_count();
};

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const noexcept {
    return _buckets_array.size();
};

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const noexcept {
    return find_node(key).second != nullptr;
}

/*
 * Implementation details: loops through each bucket,
 * and repeatedly deletes curr and sets curr to the next node.
 * At the end curr (reference to the pointer living inside the vector)
 * will be nullptr.
 *
 * HashMap is still valid after clear and can be reused, because
 * the vector is still full of nullptrs.
 */
template <typename K, typename M, typename H>
void HashMap<K, M, H>::clear() noexcept {
    for (auto& curr : _buckets_array) {
        while (curr != nullptr) {
            auto trash = curr;
            curr = curr->next;
            delete trash;
        }
    }
    _size = 0;
}

/*
 * typename is required when we need to refer to a dependent
 * qualified type. Qualified means it appears to the right of a
 * scope resolution (::), and dependent means it depends on a template.
 * The reason is that the compiler can't figure out if you are trying
 * to declare a pointer (which we are here), or trying to multiply something.
 */
template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::value_type*, bool>
HashMap<K, M, H>::insert(const value_type& value) {
    const auto& [key, mapped] = value;                  // why a reference? prevent copying
    auto [prev, node_to_edit] = find_node(key);         // copies are fine for pointers
    size_t index = _hash_function(key) % bucket_count();

    // if exists, then no-op, so return immediately
    if (node_to_edit != nullptr) return {&(node_to_edit->value), false};

    // evaluate right to left.
    // creates node, assigns its next to current next, then assigns node as the front
    _buckets_array[index] = new node(value, _buckets_array[index]);

    ++_size;
    return {&(_buckets_array[index]->value), true};
}

/*
 * We need to call find_node and get the previous and node_to_erase
 * so that we can rewire previous's next pointer.
 * 
 * It's also important to free the memory of the node!
 */
template <typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    auto [prev, node_to_erase] = find_node(key);
    if (node_to_erase == nullptr) {
        return false;
    } else {
        size_t index = _hash_function(key) % bucket_count();
        (prev ? prev->next : _buckets_array[index]) = node_to_erase->next;
        delete node_to_erase;
        --_size;
        return true;
    }
}

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
    auto [prev, node_found] = find_node(key);
    if (node_found == nullptr) {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    return node_found->value.second;
}

template <typename K, typename M, typename H>
const M& HashMap<K, M, H>::at(const K& key) const {
    auto [prev, node_found] = find_node(key);
    if (node_found == nullptr) {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    return node_found->value.second;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const {
    size_t index = _hash_function(key) % bucket_count();
    auto curr = _buckets_array[index];
    node* prev = nullptr; // if first node is the key, return {nullptr, front}
    while (curr != nullptr) {
        const auto& [found_key, found_mapped] = curr->value;
        if (found_key == key) return {prev, curr};
        prev = curr;
        curr = curr->next;
    }
    return {nullptr, nullptr}; // key not found at all.
}

/*
* Feel free to edit this function to print whatever you want.
* The autograder won't run this function.
*
* You might find it helpful to call map.debug() inside the test cases,
* particularly before a QVERIFY statement.
*/
template <typename K, typename M, typename H>
void HashMap<K, M, H>::debug() const {
    // Prints helpful debugging info using stream manipulators
    // so the formatting looks nice. Change it however you'd like.
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ')
              << "Printing debug information for your HashMap implementation\n"
              << "Size: " << size() << std::setw(15) << std::right
              << "Buckets: " << bucket_count() << std::setw(20) << std::right
              << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

    // Go through each bucket, and traverse through the linked list printing each element.
    for (size_t i = 0; i < bucket_count(); ++i) {
        std::cout << "[" << std::setw(3) << i << "]:";
        auto curr = _buckets_array[i];
        while (curr != nullptr) {
            const auto& [key, mapped] = curr->value;
            // next line will not compile if << not supported for K or M
            std::cout <<  " -> " << key << ":" << mapped;
            curr = curr->next;
        }
//        int num = 0;
//        while (curr != nullptr) {
//            // next line will not compile if << not supported for K or M
//            // std::cout <<  " -> " << key << ":" << mapped;
//            num++;
//            curr = curr->next;
//        }
//        std::cout << num << std::endl;
        std::cout <<  " /" <<  '\n';
    }
    std::cout << std::setw(30) << std::setfill('-') << '\n';
}



template <typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_bucket_count) {
    if (new_bucket_count == 0) {
        throw std::out_of_range("HashMap<K, M, H>::rehash: new_bucket_count must be positive.");
    }

    std::vector<node*> new_buckets_array(new_bucket_count);
    /* Milestone 1: begin student code */

        // Hint: you should NOT call insert, and you should not call
        // new or delete in this function. You must reuse existing nodes.
        // (void) new_buckets_array; // remove this line after you start implementing
    for (auto& curr : _buckets_array) {
        while (curr != nullptr) {
            auto old = curr;
            curr = curr->next;

            const auto& [key, mapped] = old->value;
            size_t index = _hash_function(key) % new_bucket_count;

            old->next = new_buckets_array[index];
            new_buckets_array[index] = old;
        }
    }

    _buckets_array = new_buckets_array;
    /* end student code */
}

/*
    Milestone 2-3: begin student code

    Here is a list of functions you should implement:
    Milestone 2
        - operator[]
        - operator<<
        - operator== and !=
        - make existing functions const-correct

    Milestone 3
        - copy constructor
        - copy assignment
        - move constructor
        - move assignment
*/
template <typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[] (const K& key) {
    auto [prev, node_found] = find_node(key);
    if (node_found == nullptr) {
        insert(value_type(key, M()));
    }
    auto [p, n] = find_node(key);
    return n->value.second;
}

template <typename K, typename M, typename H>
const M& HashMap<K, M, H>::operator[] (const K& key) const{
    auto [prev, node_found] = find_node(key);
    if (node_found == nullptr) {
        std::cout << __LINE__ << "   ";
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    auto [p, n] = find_node(key);
    return n->value.second;
}

template <typename k, typename m, typename h>
std::ostream& operator<<(std::ostream& os,  const HashMap<k, m, h>& map) {
    if (map.empty()){
        os << "{}";
        return os;
    }

    os << '{';
    bool flag = false;
    for (auto i = 0; i < map._buckets_array.size(); i++) {
        auto curr = map._buckets_array[i];
        while (curr != nullptr) {
            if (flag) os << ", ";

            const auto& [key, mapped] = curr->value;
            os << key << ':' << mapped;
            curr = curr->next;
            if (curr != nullptr) os << ", ";
            flag = true;
        }
    }
    os << '}';

    return os;
}

template <typename k, typename m, typename h>
bool operator== ( const HashMap<k, m, h>& lhs,  const HashMap<k, m, h>& rhs){
    if (lhs.size() != rhs.size()) return false;

//    for(auto curr : lhs._buckets_array){
//        while (curr != nullptr) {
//            auto old = curr;
//            curr = curr->next;
//
//            const auto& [key, mapped] = old->value;
//            if (!rhs.contains(key)) return false;
//            else if (lhs[key] != rhs[key]) return false;
//        }
//    }
    for (size_t i = 0; i < lhs.bucket_count(); ++i) {
        auto curr = lhs._buckets_array[i];

        while (curr != nullptr) {
            // next line will not compile if << not supported for K or M
            // std::cout <<  " -> " << key << ":" << mapped;
            const auto& [key, mapped] = curr->value;
            if (!rhs.contains(key)) return false;
            else if (lhs[key] != rhs[key]) return false;
            curr = curr->next;
        }
    }


    // std::cout << 1 << std::endl;
    return true;
}

template <typename k, typename m, typename h>
bool operator!= (const HashMap<k, m, h>& lhs, const HashMap<k, m, h>& rhs){
    return !(operator==(lhs, rhs));
}

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(const HashMap<K, M, H>& rhs) :
        _size(rhs._size),
        _hash_function(rhs._hash_function),
        _buckets_array(std::vector<node*> (rhs._buckets_array.size())){

        for(auto i = 0; i < rhs.bucket_count(); i++){
            auto curr = rhs._buckets_array[i];
            while (curr != nullptr){
                auto now = curr->value;
                insert(now).second;
                curr = curr->next;
            }
        }
        _size /=2;
//        std::copy(rhs._buckets_array.begin(), rhs._buckets_array.end(), _buckets_array.begin());
}

template <typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator = (const HashMap<K, M, H>& rhs){
    if (this == &rhs) return *this;

    std::vector<node*> new_buckets_array(rhs._buckets_array.size());
    clear();
    _size = rhs._size;
    _hash_function = rhs._hash_function;
    _buckets_array = new_buckets_array;

    for(auto i = 0; i < rhs.bucket_count(); i++){
        auto curr = rhs._buckets_array[i];
        while (curr != nullptr){
            auto now = curr->value;
            insert(now).second;
            curr = curr->next;
        }
    }
    _size /=2;

    return *this;
}

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap<K, M, H> &&rhs):
    _size(std::move(rhs._size)),
    _hash_functio
    n(std::move(rhs._hash_function)),
    _buckets_array(std::move(rhs._buckets_array)){
    for (int i = 0; i < rhs._buckets_array.size(); i++){
        rhs._buckets_array[i] = nullptr;
    }
}

template <typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator = ( HashMap<K, M, H>&& rhs){
    if(this == &rhs) return *this;

    clear();
    _size = std::move(rhs._size);
    _buckets_array = std::move(rhs._buckets_array);
    _hash_function = std::move(rhs._hash_function);
    for (int i = 0; i < rhs._buckets_array.size(); i++){
        rhs._buckets_array[i] = nullptr;
    }

    return *this;
}