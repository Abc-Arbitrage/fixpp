/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX Repeating Group
*/

#pragma once

#include <tuple>
#include <vector>
#include <sstream>

#include <fixpp/dsl/field.h>
#include <fixpp/dsl/component_block.h>
#include <fixpp/dsl/details/unwrap.h>
#include <fixpp/dsl/details/flatten.h>

namespace Fix
{

    template<template<typename> class FieldT, typename... Tags> struct MessageBase;

    // ------------------------------------------------
    // RepeatingGroup
    // ------------------------------------------------

    // A FIX repeating group

    template<typename GroupTag, typename... Tags>
    struct RepeatingGroup
    {
        using Type = GroupTag;
    };


    // ------------------------------------------------
    // Field
    // ------------------------------------------------

    // Specialization of a Field for a repeating group.
    // Stores values inside a std::vector

    template<typename GroupTag, typename... Tags>
    struct Field<RepeatingGroup<GroupTag, Tags...>>
    {
        using Tag = GroupTag;

        using Fields = typename details::flatten::tuple::Flatten<Field, Tags...>::Result;
        using Type = std::vector<Fields>;

        static constexpr size_t TotalTags = std::tuple_size<Fields>::value;

        Field() = default;
        Field(const Field& other) = default;
        Field(Field&& other) = default;

        constexpr unsigned tag() const
        {
            return Tag::Id;
        }

        const Type& get() const
        {
            return val_;
        }

        Type& get()
        {
            return val_;
        }

        size_t size() const
        {
            return val_.size();
        }

        void reserve(size_t size)
        {
            val_.reserve(size);
        }

        void push_back(const Fields& values)
        {
            val_.push_back(values);
        }

        void push_back(Fields&& values)
        {
            val_.push_back(std::move(values));
        }

        bool empty() const
        {
            return val_.empty();
        }

    private:
        Type val_;
    };

    // ------------------------------------------------
    // GroupRef
    // ------------------------------------------------

    template<typename Group> struct GroupRef;

    template<typename GroupTag, typename... Tags>
    struct GroupRef<RepeatingGroup<GroupTag, Tags...>> : public MessageBase<FieldRef, Tags...>
    {
    };

    // ------------------------------------------------
    // FieldRef
    // ------------------------------------------------

    // Specialization of a FieldRef for a repeating group.
    // Represents a "view" on a repeating group

    template<typename GroupTag, typename... Tags>
    struct FieldRef<RepeatingGroup<GroupTag, Tags...>>
    {
        using Tag = GroupTag;

        using RefType = GroupRef<RepeatingGroup<GroupTag, Tags...>>;
        using Values = std::vector<RefType>;

        constexpr unsigned tag() const
        {
            return GroupTag::Id;
        }

        template<typename TypeRef>
        void add(TypeRef&& ref)
        {
            values.push_back(std::forward<TypeRef>(ref));
        }

        void reserve(size_t size)
        {
            values.reserve(size);
        }

        Values get() const
        {
            return values;
        }

    private:
        Values values;
    };

    // ------------------------------------------------
    // Group
    // ------------------------------------------------

    template<typename RepeatingGroup> struct Group;

    template<typename GroupTag, typename... Tags>
    struct Group<RepeatingGroup<GroupTag, Tags...>>
    {
        using Instance = MessageBase<Field, Tags...>;
        using FieldType = Field<RepeatingGroup<GroupTag, Tags...>>;

        Group(FieldType& field)
            : field(field)
        {
        }

        Instance instance() const
        {
            return { };
        }

        void add(const Instance& instance)
        {
            checkRequiredFields(instance);
            field.push_back(instance.values);
        }

        void add(Instance&& instance)
        {
            checkRequiredFields(instance);
            field.push_back(std::move(instance.values));
        }

        size_t size() const
        {
            return field.size();
        }

    private:
        template<typename InstanceT> void checkRequiredFields(InstanceT&& instance) const
        {
            if (!instance.requiredBits.all())
            {
                std::ostringstream error;
                const size_t missingBits = instance.requiredBits.size() - instance.requiredBits.count();
                error << "Missing " << missingBits << " required value(s) for Instance";
                throw std::runtime_error(error.str());
            }
        }

        FieldType& field;
    };

} // namespace Fix
