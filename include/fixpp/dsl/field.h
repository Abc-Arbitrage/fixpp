/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX field
*/

#pragma once

#include <tuple>
#include <type_traits>

#include <fixpp/meta.h>
#include <fixpp/dsl/details/lexical_cast.h>

namespace Fixpp
{
 
    // ------------------------------------------------
    // Field
    // ------------------------------------------------

    // A Field that stores its value

    template<typename TagT>
    struct Field
    {
        using Tag = TagT;
        using Type = typename TagT::Type::UnderlyingType;

        Field()
            : empty_(true)
        { }

        Field(const Field& other)
        {
            operator=(other);
        }

        Field(Field&& other)
        {
            operator=(std::move(other));
        }

        Field& operator=(const Field& other)
        {
            if (!other.empty())
                val_ = other.val_;

            empty_ = other.empty_;
            return *this;
        }

        Field& operator=(Field&& other)
        {
            if (!other.empty())
            {
                val_ = std::move(other.val_);
                empty_ = false;
                other.empty_ = true;
            }
            else
            {
                empty_ = true;
            }
            return *this;
        }

        template<typename T>
        void set(T&& value)
        {
            val_ = std::forward<T>(value);
            empty_ = false;
        }

        const Type& get() const
        {
            return val_;
        }

        bool empty() const
        {
            return empty_;
        }

    private:        
        Type val_;
        bool empty_;
    };

    // ------------------------------------------------
    // FieldRef
    // ------------------------------------------------

    // A FieldRef stores a "view" of a field inside a frame.
    // The real value will be parsed lazily

    template<typename TagT>
    struct FieldRef
    {
        using Tag = TagT;
        using Type = typename TagT::Type::UnderlyingType;

        FieldRef() = default;

        constexpr unsigned tag() const
        {
            return Tag::Id;
        }

        void set(const std::pair<const char*, size_t>& value)
        {
            offset = value.first;
            size = value.second;
        }

        Type get() const
        {
            return details::LexicalCast<typename TagT::Type>::cast(offset, size);
        }

        bool valid() const
        {
            return offset != nullptr;
        }

    private:
        const char* offset;
        size_t size;
    };


} // namespace Fixpp
