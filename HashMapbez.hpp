//
// Created  on 11/01/2023.
//
#include <vector>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <utility>
#include <algorithm>
//#include <iterator>


#define MINIMUMSIZE 16
#define UPTHRESHOLD 0.75
#define DOWNTHRESHOLD 0.25
#ifndef _HASHMAP_HPP_
#define _HASHMAP_HPP_
int start_capacity_cal (int size_of_vector)
{
  int current = MINIMUMSIZE;
  while (static_cast<double>(size_of_vector) / current >= UPTHRESHOLD)
  {
    current *= 2;
  }
  return current;
}

template<typename KeyT, typename ValueT>
class HashMap
{ 
  typedef std::pair<KeyT, ValueT> Pair_t;
  typedef std::vector<Pair_t> bucket;
  typedef bucket *table;
 public:
  HashMap () : key_t (new bucket[MINIMUMSIZE] ())
  {
    num_of_evarim = 0, capacity_val = MINIMUMSIZE;
  }

  HashMap (std::vector<KeyT> keys, std::vector<ValueT> values)
  {
    if (keys.size () != values.size ())
    {
      throw std::invalid_argument ("vectors not the same size");
    }
    num_of_evarim = 0;
    capacity_val = start_capacity_cal (keys.size ());
    key_t  = new bucket[capacity_val] ();
    int i = 0;
    for (auto &key: keys)
    {
      if (!contains_key (key))
      {
        auto pair = std::make_pair (keys[i], values[i]);
        int hash_num = hash_func (pair.first);

        key_t[hash_num].push_back (pair);
        ++num_of_evarim;
      }
      else
      {
        this->operator[] (key) = values[i];
      }
      ++i;
    }
  }
  HashMap (const HashMap<KeyT, ValueT> &a)
  {
    num_of_evarim = a.num_of_evarim;
    key_t = new bucket[a.capacity ()];
    for (int i = 0; i < a.capacity (); i++)
    {
      key_t[i] = a.key_t[i];
    }
    capacity_val = a.capacity_val;
  }

  int size () const
  {
    return num_of_evarim;
  }

  int capacity () const
  {
    return capacity_val;
  }

  bool empty () const
  {
    return num_of_evarim == 0;
  }

  bool insert (KeyT key, ValueT val)
  {
    if (!contains_key (key))
    {
      auto pair = std::make_pair (key, val);
      int hash_num = hash_func (pair.first);
      key_t[hash_num].push_back (pair);
      ++num_of_evarim;
      if (get_load_factor () > UPTHRESHOLD)
      {
        re_hash ();
      }
      return true;
    }
    return false;
  }

  bool contains_key (const KeyT &k) const
  {
    try
    {
      at (k);
    }
    catch (std::exception &)
    {
      return false;
    }
    return true;
  }

  ValueT &at (const KeyT &k) const
  {
    auto hash_k = hash_func (k);
    for (auto &pair: key_t[hash_k])
    {
      if (k == pair.first)
      {
        return pair.second;
      }
    }
    throw std::runtime_error ("no such value");
  }

  virtual bool erase (const KeyT &k)
  {
    auto hash_k = hash_func (k);
    for (auto &pair: key_t[hash_k])
    {
      if (k == pair.first)
      {
        key_t[hash_k].erase (std::find (key_t[hash_k].begin (),
                                        key_t[hash_k].end (), pair));
        --num_of_evarim;
        if (get_load_factor () < DOWNTHRESHOLD)
        {
          re_hash (false);
        }
        return true;
      }
    }
    return false;
  }

  double get_load_factor () const
  {
    return (double) num_of_evarim / capacity_val;
  }

  int bucket_size (const KeyT &key) const
  {
    if (!contains_key (key))
    {
      throw std::runtime_error ("bucket_size- key is not in table");
    }
    auto hash_k = hash_func (key);
    return key_t[hash_k].size ();
  }

  int bucket_index (const KeyT &key) const
  {
    if (!contains_key (key))
    {
      throw std::runtime_error ("bucket_index- key is not in table");
    }
    return hash_func (key);
  }

  void clear () noexcept
  {
    for (int i = 0; i < capacity_val; ++i)
    {
      key_t[i].clear ();
      num_of_evarim = 0;
    }
  }

  class ConstIterator
  {
   public:
    typedef const Pair_t value_type;
    typedef const Pair_t &reference;
    typedef const Pair_t *pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;

    explicit ConstIterator (const table &key_t, int capaci, bool end = false)
        : key_t_p (key_t), capaci (capaci)
    {
      j = 0;
      end ? bucket_num = capaci : bucket_num = 0;
      while (key_t_p[bucket_num].empty () && bucket_num < capaci)
      {
        bucket_num++;
      }
      end ? m_ptr = nullptr : m_ptr = &key_t_p[bucket_num][0];
    }

    const Pair_t &operator* () const
    { return *m_ptr; }
    const Pair_t *operator-> ()
    { return m_ptr; }
    ConstIterator &operator++ ()
    {
      if (j + 1 != key_t_p[bucket_num].size ())
      {
        j++, m_ptr++;
        return *this;
      }
      else
      {
        bucket_num++;
        if (bucket_num >= capaci)
        {
          return *this;
        }
        while (key_t_p[bucket_num].empty ())
        {
          bucket_num++;
          if (bucket_num >= capaci)
          {
            return *this;
          }
        }
        m_ptr = &key_t_p[bucket_num][0];
        return *this;
      }
    }
    ConstIterator operator++ (int)
    {
      ConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator== (const ConstIterator &a, const ConstIterator &b)
    {
      if (a.bucket_num == a.capaci && b.bucket_num == a.capaci)
      {
        return true;
      }
      return a.bucket_num == b.bucket_num && a.j == b.j;
    };
    friend bool operator!= (const ConstIterator &a, const ConstIterator &b)
    {
      return !(a == b);
    };

   private:
    const Pair_t *m_ptr;
    const table &key_t_p;
    int bucket_num = 0;
    int j, capaci;
  };

  typedef ConstIterator const_iterator;
  const_iterator begin()
  {
    return const_iterator (key_t, capacity_val);
  }
  const_iterator end()
  {
    return const_iterator (key_t, capacity_val, true);
  }
  const_iterator begin () const
  {
    return const_iterator (key_t, capacity_val);
  }
  const_iterator end () const
  {
    return const_iterator (key_t, capacity_val, true);
  }
  const_iterator cbegin() const
  {
    return begin();
  }
  const_iterator cend() const
  {
    return end();
  }

  HashMap<KeyT, ValueT> &operator= (const HashMap<KeyT, ValueT> &b)
  {
    auto new_table = new bucket[b.capacity ()];
    for (int i = 0; i < b.capacity (); i++)
    {
      new_table[i] = b.key_t[i];
    }
    delete[] this->key_t;
    this->key_t = new_table, this->num_of_evarim = b.num_of_evarim;
    this->capacity_val = b.capacity_val;
    return *this;
  }

  ValueT &operator[] (const KeyT key)
  {
    try
    {
      return at (key);
    }
    catch (std::runtime_error &a)
    {
      Pair_t pair = std::make_pair (key, ValueT ());
      int hash_num = hash_func (pair.first);

      key_t[hash_num].push_back (pair);
      ++num_of_evarim;
      return at (key);
    }
  }

  bool operator == (const HashMap<KeyT, ValueT> &b)
  {
    for (auto &pair: *this)
    {
      try
      {
        if (b.at (pair.first) != pair.second)
        {
          return false;
        }
      }
      catch (std::runtime_error &a)
      {
        return false;
      }
    }
    return true;
  }

  bool operator!= (const HashMap<KeyT, ValueT> &b)
  {
    bool ans = (this->operator== (b));
    return !ans;
  }

 protected:

  int hash_func (KeyT key) const
  {
    //std::size_t hash_val = std::hash<KeyT>{}(key);
    std::size_t hash_val = std::hash<KeyT> () (key);
    return hash_val & (capacity_val - 1);
  }

  void re_hash (bool increase = true)
  {
    int new_capacity = capacity_val;
    if (increase)
    {
      new_capacity *= 2;
    }
    else
    {
      new_capacity /= 2;
    }
    auto new_table = new bucket[new_capacity]();
    capacity_val = new_capacity;
    auto old(this);
    key_t = new_table;
    for (auto &pair: *old)
    {
      int hash_num = hash_func (pair.first);
      new_table[hash_num].push_back (pair);
    }
    delete[] key_t;
    key_t = new_table;
  }
  table key_t;
  int num_of_evarim;
  int capacity_val;
 private:

};


#endif //_HASHMAP_HPP_



