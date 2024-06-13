#ifndef _HASHMAP_HPP_
#define _HASHMAP_HPP_

#include <vector>
#include <stdexcept>

#define DEFAULT_CAPACITY 16
#define MAX_LOAD_FACTOR 0.75
#define MIN_LOAD_FACTOR 0.25
#define INITIAL_INT 0
#define RESIZE_FACTOR 2
#define MESSAGE_KEY_NOT_FOUND "Key not found"
#define MESSAGE_UNMATCHED_SIZE "Keys and values are not of the same size"

template<typename KeyT, typename ValueT>
class HashMap
{

 protected:
  template<class Pair>
  class IteratorConst;

 public:
  typedef IteratorConst<const std::pair<KeyT, ValueT>> Iterator;

  /**
   * Default constructor
   */
  HashMap ()
  {
    size_ = INITIAL_INT;
    capacity_ = DEFAULT_CAPACITY;
    load_factor_ = INITIAL_INT;
    hash_table_ = new bucket[capacity_];
  }

  /**
*    Constructor that takes two vectors of the same size and
*    creates a hash map.
   * @param keys vector of keys
   * @param values vector of values
   */
  HashMap (const std::vector<KeyT> keys, const std::vector<ValueT> values)
  {
    if (keys.size () != values.size ())
    {
      throw std::invalid_argument (MESSAGE_UNMATCHED_SIZE);
    }
    size_ = INITIAL_INT;
    capacity_ = DEFAULT_CAPACITY;
    load_factor_ = INITIAL_INT;
    hash_table_ = new std::vector<std::pair<KeyT, ValueT>>[capacity_];
    for (unsigned long i = 0; i < keys.size (); ++i)
    {
      this->operator[] (keys[i]) = values[i];
    }
  }

  /**
   * Copy constructor
   * @param other the other hash map.
   */
  HashMap (const HashMap &other)
  {
    size_ = other.size_;
    capacity_ = other.capacity_;
    load_factor_ = other.load_factor_;
    hash_table_ = new bucket[capacity_];
    for (int i = 0; i < capacity_; i++)
    {
      hash_table_[i] = other.hash_table_[i];
    }
  }

  /**
   * Destructor
   */
  virtual ~HashMap ()
  {
    delete[] hash_table_;
  }

  /**
   * This method returns the size of the hash map.
   * @return size of the hash map.
   */
  int size () const
  {
    return size_;
  }

  /**
   * This method returns the capacity of the hash map.
   * @return capacity of the hash map.
   */
  int capacity () const
  {
    return capacity_;
  }

  /**
   * This method check if the hash map is empty.
   * @return true if the hash map is empty, false otherwise.
   */
  bool empty () const
  {
    return size_ == INITIAL_INT;
  }

  /**
   * This method insert a key-value pair into the hash map.
   * @param key
   * @param value
   * @return true if the key-value pair is inserted, false otherwise.
   */
  bool insert (const KeyT key, const ValueT value)
  {
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    for (size_t i = 0; i < hash_table_[hash].size (); i++)
    {
      if (hash_table_[hash][i].first == key)
      {
        return false;
      }
    }
    hash_table_[hash].push_back (std::make_pair (key, value));
    size_++;
    load_factor_ = (double) size_ / capacity_;
    if (load_factor_ > MAX_LOAD_FACTOR)
    {
      resize (true);
      load_factor_ = (double) size_ / capacity_;
    }
    return true;
  }

  /**
   * This method check if a key is in the hash map.
   * @param key
   * @return true if the key is in the hash map, false otherwise.
   */
  bool contains_key (const KeyT key) const
  {
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    for (size_t i = 0; i < hash_table_[hash].size (); i++)
    {
      if (hash_table_[hash][i].first == key)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * This method returns the value of a key.
   * @param key
   * @return if the key is in the hash map, return the value of the key,
   * otherwise, throw an exception.
   */
  ValueT &at (const KeyT key)
  {
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    for (size_t i = 0; i < hash_table_[hash].size (); i++)
    {
      if (hash_table_[hash][i].first == key)
      {
        return hash_table_[hash][i].second;
      }
    }
    throw std::out_of_range (MESSAGE_KEY_NOT_FOUND);
  }

  /**
   * This method returns the value of a key. This method is const.
   * @param key
   * @return if the key is in the hash map, return the value of the key,
   * otherwise, throw an exception.
   */
  const ValueT &at (const KeyT key) const
  {
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    for (size_t i = 0; i < hash_table_[hash].size (); i++)
    {
      if (hash_table_[hash][i].first == key)
      {
        return hash_table_[hash][i].second;
      }
    }
    throw std::out_of_range (MESSAGE_KEY_NOT_FOUND);
  }

  /**
   * This method erase a key-value pair from the hash map.
   * @param key
   * @return if the key is in the hash map, erase the key-value pair and
   * return true, otherwise, return false.
   */
  virtual bool erase (const KeyT key)
  {
    bool found = false;
    if (empty ())
    {
      return false;
    }
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    for (size_t i = 0; i < hash_table_[hash].size (); i++)
    {
      if (hash_table_[hash][i].first == key)
      {
        hash_table_[hash][i] = hash_table_[hash][hash_table_[hash].size ()
                                                 - 1];
        hash_table_[hash].pop_back ();
        size_ -= 1;
        load_factor_ = (double) size_ / capacity_;
        found = true;
        if (empty ())
        {
          this->capacity_ = 1;
          return true;
        }
      }
    }
    if (found)
    {
      if (load_factor_ < MIN_LOAD_FACTOR)
      {
        resize (false);
        load_factor_ = (double) size_ / capacity_;
      }
      return true;
    }
    return false;
  }

  /**
   * This method returns the load factor of the hash map.
   * @return load factor of the hash map.
   */
  double get_load_factor () const
  {
    return load_factor_;
  }

  /**
   * This method returns the size of the bucket at the given key.
   * @param key
   * @return size of the bucket at the given key.
   */
  int bucket_size (const KeyT key) const
  {
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    if (contains_key (key))
    {
      return hash_table_[hash].size ();
    }
    throw std::invalid_argument (MESSAGE_KEY_NOT_FOUND);
  }

  /**
   * This method returns the index of the bucket at the given key.
   * @param key
   * @return the index of the bucket at the given key.
   */
  int bucket_index (const KeyT key) const
  {
    size_t hash = std::hash<KeyT>{} (key) & (capacity_ - 1);
    if (contains_key (key))
    {
      return (int) hash;
    }
    throw std::invalid_argument (MESSAGE_KEY_NOT_FOUND);
  }

  /**
   * This method removes all the items from the hash map.
   */
  void clear ()
  {
    delete[] hash_table_;
    size_ = INITIAL_INT;
    load_factor_ = INITIAL_INT;
    hash_table_ = new std::vector<std::pair<KeyT, ValueT>>[capacity_];
  }

 protected:
  typedef std::pair<KeyT, ValueT> Pair;
  typedef std::vector<Pair> bucket;
  int capacity_;
  int size_;
  double load_factor_;
  bucket *hash_table_;

  /**
   * This is nested class for iterator.
   */
  template<class Pair>
  class IteratorConst
  {
    friend class HashMap;
   public:
    typedef const std::forward_iterator_tag iterator_category;
    typedef const Pair value_type;
    typedef const Pair &reference;
    typedef const Pair *pointer;
    typedef const std::ptrdiff_t difference_type;

    /**
     * This is the default constructor.
     */
    explicit IteratorConst (const HashMap<KeyT, ValueT> &hash_map,
                            bool end = false) : hash_map_ (hash_map)
    {
      if (end)
      {
        bucket_index_ = hash_map_.capacity_;
        bucket_element_index_ = INITIAL_INT;
      }
      else
      {
        bucket_index_ = INITIAL_INT;
        bucket_element_index_ = INITIAL_INT;
        while (bucket_index_ < hash_map_.capacity_
               && hash_map_.hash_table_[bucket_index_].empty ())
        {
          bucket_index_ += 1;
        }
      }
    }

    /**
     * This is operator*. It returns the value of the iterator.
     * @return
     */
    value_type &operator* () const
    {
      return hash_map_.hash_table_[bucket_index_][bucket_element_index_];
    }

    /**
     * This is operator->. It returns the pointer of the iterator.
     * @return
     */
    pointer operator-> () const
    {
      return &hash_map_.hash_table_[bucket_index_][bucket_element_index_];
    }

    /**
     * This is operator++. It returns the next iterator.
     * @return the next iterator.
     */
    IteratorConst &operator++ ()
    {
      bucket_element_index_ += 1;
      if ((unsigned long) bucket_element_index_
          == hash_map_.hash_table_[bucket_index_].size ())
      {
        bucket_index_ += 1;
        while (bucket_index_ < hash_map_.capacity_
               && hash_map_.hash_table_[bucket_index_].empty ())
        {
          bucket_index_ += 1;
        }
        bucket_element_index_ = INITIAL_INT;
      }
      return *this;
    }

    /**
     * This is operator++. It returns the current iterator and then
     * increment the iterator.
     * @return the current iterator.
     */
    IteratorConst operator++ (int)
    {
      IteratorConst tmp = *this;
      ++(*this);
      return tmp;
    }

    /**
     * This is operator==. It returns true if the two iterators are equal.
     * @param other
     * @return true if the two iterators are equal. Otherwise, return false.
     */
    bool operator== (const IteratorConst &other) const
    {
      return bucket_index_ == other.bucket_index_
             && bucket_element_index_ == other.bucket_element_index_
             && &hash_map_ == &other.hash_map_;
    }

    /**
     * This is operator!=. It returns true if the two iterators are not
     * equal.
     * @param other
     * @return true if the two iterators are not equal. Otherwise, return
     * false.
     */
    bool operator!= (const IteratorConst &other) const
    {
      return !(*this == other);
    }

   protected:
    const HashMap &hash_map_;
    int bucket_index_;
    int bucket_element_index_;

  };

 public:

  /**
   * This is cbegin method.
   * @return It returns the iterator to the beginning.
   */
  Iterator cbegin () const
  {
    return IteratorConst<const Pair> (*this);
  }

  /**
   * This is cend method.
   * @return It returns the iterator to the end.
   */
  Iterator cend () const
  {
    return IteratorConst<const Pair> (*this, true);
  }

  /**
 * This is begin method.
 * @return It returns the iterator to the beginning.
 */
  Iterator begin () const
  {
    return IteratorConst<const Pair> (*this);
  }

  /**
   * This is end method.
   * @return It returns the iterator to the end.
   */
  Iterator end () const
  {
    return IteratorConst<const Pair> (*this, true);
  }

  /**
   * This function swap the two hash maps.
   * @param other the other hash map.
   */
  void swap (HashMap &other)
  {
    std::swap (capacity_, other.capacity_);
    std::swap (size_, other.size_);
    std::swap (load_factor_, other.load_factor_);
    std::swap (hash_table_, other.hash_table_);
  }

  /**
   * This is assignment operator. It assigns the other hash map to this.
   * @param other the other hash map.
   * @return the reference to this hash map.
   */
  HashMap &operator= (HashMap other)
  {
    swap (other);
    return *this;
  }

  /**
   * This is operator[]. It returns the value of the key.
   * @param key the key.
   * @return the value of the key.
   */
  ValueT &operator[] (const KeyT &key)
  {
    if (contains_key (key))
    {
      return at (key);
    }
    else
    {
      insert (key, ValueT ());
      return at (key);
    }
  }

  /**
   * This is operator[]. It returns the value of the key. It is const
   * @param key the key.
   * @return the value of the key.
   */
  const ValueT &operator[] (const KeyT &key) const
  {
    if (contains_key (key))
    {
      return at (key);
    }
    else
    {
      return DEFAULT_VALUE;
    }
  }

  /**
   * This is operator==. It returns true if the two hash maps are equal,
   * Otherwise, return false.
   * @param other the other hash map.
   * @return true if the two hash maps are equal, Otherwise, return false.
   */
  bool operator== (const HashMap &other) const
  {
    try
    {
      if (size_ != other.size_)
      {
        return false;
      }
      for (auto it = cbegin (); it != cend (); ++it)
      {
        if (other.at (it->first) != it->second)
        {
          return false;
        }
      }
      return true;
    }
    catch (std::out_of_range &error_msg)
    {
      return false;
    }
  }

  /**
   * This is operator!=. It returns true if the two hash maps are not equal.
   * @param other the other hash map.
   * @return true if the two hash maps are not equal. Otherwise, return false.
   */
  bool operator!= (const HashMap &other) const
  {
    return !(*this == other);
  }

  /**
   * Thus function resizes the hash map.
   * @param is_up is boolean value. If it is true, it increases the size of
   * the hash map, otherwise, it decreases the size of the hash map.
   */
  void resize (bool is_up)
  {
    int new_capacity = capacity_;
    if (is_up)
    {
      new_capacity *= RESIZE_FACTOR;
    }
    else
    {
      new_capacity /= RESIZE_FACTOR;
    }
    auto *new_hash_table = new bucket[new_capacity];
    for (int i = 0; i < capacity_; ++i)
    {
      for (auto &item: hash_table_[i])
      {
        int hash_num = std::hash<KeyT>{} (item.first) & (new_capacity - 1);
        new_hash_table[hash_num].push_back (item);
      }
    }
    delete[] hash_table_;
    hash_table_ = new_hash_table;
    capacity_ = new_capacity;
  }

 private:
  ValueT DEFAULT_VALUE;

};
#endif //_HASHMAP_HPP_