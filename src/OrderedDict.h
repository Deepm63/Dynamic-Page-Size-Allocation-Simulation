#include <iostream>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::unordered_map;
using std::vector;
using std::find;

/**
 * @brief A dictionary data structure that maintains insertion order of keys
 * 
 * OrderedDict combines the fast lookup capabilities of unordered_map with
 * the ability to maintain and iterate over keys in their insertion order.
 * It uses an unordered_map for O(1) average-case key-value operations and
 * a vector to track the order of key insertions.
 * 
 * @tparam Key The type of keys stored in the dictionary
 * @tparam Value The type of values stored in the dictionary
 */
template <typename Key, typename Value>
class OrderedDict
{
private:
    unordered_map<Key, Value> map;
    vector<Key> order;

public:
    /**
     * @brief Inserts a key-value pair into the dictionary
     * 
     * If the key already exists, updates its value without changing its position
     * in the insertion order. If the key is new, adds it to the end of the order.
     * 
     * @param key The key to insert or update
     * @param value The value to associate with the key
     */
    void insert(const Key &key, const Value &value)
    {
        if (map.find(key) == map.end())
        {
            order.push_back(key);
        }
        map[key] = value;
    }

    /**
     * @brief Provides access to values by key with automatic insertion
     * 
     * Returns a reference to the value associated with the given key.
     * If the key doesn't exist, creates a new entry with default-constructed
     * value and adds the key to the end of the insertion order.
     * 
     * @param key The key to access
     * @return Reference to the value associated with the key
     */
    Value &operator[](const Key &key)
    {
        if (map.find(key) == map.end())
        {
            order.push_back(key); // Add to order if new key accessed
        }
        return map[key];
    }

    /**
     * @brief Removes a key-value pair from the dictionary
     * 
     * Deletes both the key-value mapping and removes the key from the
     * insertion order vector. If the key doesn't exist, no operation is performed.
     * 
     * @param key The key to remove from the dictionary
     */
    void erase(const Key &key)
    {
        auto it = map.find(key);
        if (it != map.end())
        {
            map.erase(it);
            order.erase(find(order.begin(), order.end(), key));
        }
    }

    /**
     * @brief Returns the insertion order of keys
     * 
     * Provides read-only access to the vector containing all keys
     * in their insertion order.
     * 
     * @return Const reference to the vector of keys in insertion order
     */
    const vector<Key> &get_order() const
    {
        return order;
    }

    /**
     * @brief Prints all key-value pairs in insertion order
     * 
     * Iterates through the dictionary in insertion order and prints
     * each key-value pair to standard output in "key: value" format.
     */
    void print_in_order() const
    {
        for (const auto &key : order)
        {
            cout << key << ": " << map.at(key) << endl;
        }
    }

    /**
     * @brief Checks if a key exists in the dictionary
     * 
     * Determines whether the specified key is present in the dictionary
     * without modifying the data structure.
     * 
     * @param key The key to search for
     * @return True if the key exists, false otherwise
     */
    bool contains(const Key &key) const
    {
        return map.find(key) != map.end();
    }

    /**
     * @brief Moves an existing key to the end of the insertion order
     * 
     * Relocates the specified key to the end of the insertion order vector,
     * effectively making it the most recently inserted key. If the key
     * doesn't exist in the order, no operation is performed.
     * 
     * @param key The key to move to the end of the order
     */
    void move_to_end(const Key &key)
    {
        auto it = find(order.begin(), order.end(), key);
        if (it != order.end())
        {
            order.erase(it);
            order.push_back(key);
        }
    }

    /**
     * @brief Returns the number of key-value pairs in the dictionary
     * 
     * @return The total count of elements stored in the dictionary
     */
    size_t size() const
    {
        return map.size();
    }
};