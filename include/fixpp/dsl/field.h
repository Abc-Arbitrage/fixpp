/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX field
*/

#pragma once

#include <tuple>
#include <type_traits>

#include <fixpp/meta.h>
#include <fixpp/view.h>
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
        using Type = typename TagT::Type::StorageType;

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
        using Type = typename TagT::Type::StorageType;

        static constexpr bool IsRef = false;

        FieldRef() = default;

        FieldRef(const FieldRef& other) = default;
        FieldRef& operator=(const FieldRef& other) = default;

        FieldRef(FieldRef&& other) = default;
        FieldRef& operator=(FieldRef&& other) = default;

        constexpr unsigned tag() const
        {
            return Tag::Id;
        }

        void set(const View& view)
        {
            m_view = view;
        }

        Type get() const
        {
            return details::LexicalCast<typename TagT::Type>::cast(m_view.first, m_view.second);
        }

        View view() const
        {
            return m_view;
        }

        bool valid() const
        {
            return m_view.first != nullptr;
        }

    private:
        View m_view;
    };

} // namespace Fixpp
