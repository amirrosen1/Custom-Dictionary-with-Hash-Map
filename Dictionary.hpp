#ifndef _DICTIONARY_HPP_
#define _DICTIONARY_HPP_
#include <utility>
#include <vector>
#include <stdexcept>
#include <string>
#include "HashMap.hpp"

#define INVALID_KEY_MSG "Invalid key"

/**
 * A class of InvalidKey. This class is used to throw an exception. This class
 * inherits from the std::invalid_argument class.
 */
class InvalidKey : public std::invalid_argument
{
 public:
  /**
   * An empty constructor of InvalidKey. This constructor calls the
   * constructor of std::invalid_argument.
   */
  InvalidKey () : std::invalid_argument (INVALID_KEY_MSG)
  {}

/**
 * A constructor of InvalidKey. This constructor calls the constructor of
 * std::invalid_argument.
 * @param error_msg message error.
 */
  explicit InvalidKey (const std::string &error_msg_)
      : std::invalid_argument (error_msg_)
  {
  }

 protected:
  std::string error_msg_;

};

/**
 * Dictionary class. This class is used to create a dictionary object.
 * This class inherits from the HashMap class, so that the keys and values
 * are strings.
 */
class Dictionary : public HashMap<std::string, std::string>
{
 public:

  /**
   * An empty constructor of Dictionary.
   */
  Dictionary ()
  = default;

  /**
   * A constructor of Dictionary. This constructor calls the constructor
   * of HashMap.
   * @param Key_Vector this is a vector of keys.
   * @param Value_Vector this is a vector of values.
   */
  Dictionary (std::vector<std::string> Key_Vector,
              std::vector<std::string> Value_Vector)
      : HashMap (std::move (Key_Vector), std::move (Value_Vector))
  {
  }

/**
   * This method get a key and if the key is in the dictionary,
   * it erase the value associated with the key.
   * If the key is not in the dictionary, it throws an exception.
   * This method is inherited from the HashMap class, and so it is overridden.
   * @param Key The key.
   * @return True if the erase was successful, otherwise it throws an
   * exception.
   */
  bool erase (std::string Key) override
  {
    if (!HashMap<std::string, std::string>::contains_key (Key))
    {
      throw InvalidKey (MESSAGE_KEY_NOT_FOUND);
    }
    return HashMap<std::string, std::string>::erase (Key);
  }

  /**
   * This method get two iterators and it updates the dictionary with the
   * items that are in this range.
   * @tparam ForwardIterator The type of the iterators.
   * @param first The first iterator.
   * @param last The last iterator.
   */
  template<class ForwardIterator>
  void update (ForwardIterator first, ForwardIterator last)
  {
    if (first == last)
    {
      return;
    }
    while (first != last)
    {
      if (contains_key (first->first))
      {
        this->operator[] (first->first) = first->second;
      }
      else
      {
        insert (first->first, first->second);
      }
      ++first;
    }
  }

};
#endif //_DICTIONARY_HPP_