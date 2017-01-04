/* message.h
   Mathieu Stefani, 04 january 2017
   
  A representation of a FIX Repeating Group
*/

#pragma once

#include <tuple>
#include <vector>

#include <fixpp/dsl/field.h>
#include <fixpp/dsl/details/unwrap.h>

namespace Fix
{

    template<template<typename> class FieldT, typename... Tags> struct MessageBase;

    template<typename GroupTag, typename... Tags>
    struct RepeatingGroup
    {
        using Type = GroupTag;
    };

    template<typename GroupTag, typename... Tags>
    struct Field<RepeatingGroup<GroupTag, Tags...>>
    {
        using Tag = GroupTag;
        using GroupValues = std::tuple<Field<typename details::Unwrap<Tags>::Result>...>;
        using Type = std::vector<GroupValues>;

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

        void push_back(const GroupValues& values)
        {
            val_.push_back(values);
        }

        void push_back(GroupValues&& values)
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

    template<typename Group> struct GroupRef;

    template<typename GroupTag, typename... Tags>
    struct GroupRef<RepeatingGroup<GroupTag, Tags...>> : public MessageBase<FieldRef, Tags...>
    {
    };

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
            return Instance { };
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
