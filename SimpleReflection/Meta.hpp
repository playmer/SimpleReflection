#pragma once

#include "SimpleReflection/TypeTraits.hpp"
#include "SimpleReflection/Type.hpp"
#include "SimpleReflection/Any.hpp"
#include "SimpleReflection/Function.hpp"
#include "SimpleReflection/Field.hpp"
#include "SimpleReflection/Property.hpp"
#include "SimpleReflection/Attribute.hpp"

namespace srefl
{
  inline constexpr std::nullptr_t NoGetter = nullptr;
  inline constexpr std::nullptr_t NoSetter = nullptr;
  
  template <typename tFunctionSignature>
  constexpr auto SelectOverload(tFunctionSignature aBoundFunction) -> tFunctionSignature
  {
    return aBoundFunction;
  }


  template <typename tType>
  class TypeBuilder
  {
    public:
    TypeBuilder()
      : mType{ TypeId<tType>() }
    {

    }

    Type* GetType()
    {
      return mType;
    }

    template <typename tFunctionSignature>
    class FunctionBuilder
    {
      public:
      FunctionBuilder(srefl::Function* aFunction)
        : mFunction{ aFunction }
      {

      }

      srefl::Function* GetFunction()
      {
        return mFunction;
      }

      template <typename... tArguments>
      FunctionBuilder& SetParameterNames(tArguments&& ...aNames)
      {
        constexpr size_t passedNames = sizeof...(aNames);
        constexpr size_t funcNames = CountFunctionArguments<tFunctionSignature>::Size();
        static_assert(0 != funcNames, "You needn't set the parameter names for this function, as there are no parameters.");

        static_assert(passedNames == funcNames,
                      "If passing names of function parameters you must pass either exactly as many names as there are arguments, or 0.");

        mFunction->SetParameterNames({ std::forward<tArguments>(aNames)... });

        return *this;
      }

      FunctionBuilder& SetDocumentation(char const* aString)
      {
        mFunction->SetDocumentation(aString);

        return *this;
      }

      template <typename tAttributeType, typename... tArguments>
      FunctionBuilder& AddAttribute(tArguments&& ...aArguments)
      {
        mFunction->AddAttribute<tAttributeType>(std::forward<tArguments>(aArguments)...);

        return *this;
      }

      private:
      srefl::Function* mFunction;
    };

    template <auto tBoundFunction>
    auto Function(char const* aName)
    {
      using FunctionSignature = decltype(tBoundFunction);
      auto function = Detail::Meta::FunctionBinding<FunctionSignature>:: template BindFunction<tBoundFunction>(aName);
      function->SetOwningType(TypeId<tType>());

      auto ptr = TypeId<tType>()->AddFunction(std::move(function));

      return FunctionBuilder<FunctionSignature>{ ptr };
    }

    template <auto tGetterFunction, auto tSetterFunction>
    srefl::Property& Property(char const* aName)
    {
      using GetterFunctionSignature = decltype(tGetterFunction);
      using SetterFunctionSignature = decltype(tSetterFunction);

      std::unique_ptr<srefl::Function> getter;
      std::unique_ptr<srefl::Function> setter;

      getter = Detail::Meta::FunctionBinding<GetterFunctionSignature>:: template BindFunction<tGetterFunction>("Getter");
      setter = Detail::Meta::FunctionBinding<SetterFunctionSignature>:: template BindFunction<tSetterFunction>("Setter");

      auto property = std::make_unique<srefl::Property>(aName, std::move(getter), std::move(setter));

      auto ptr = TypeId<tType>()->AddProperty(std::move(property));

      return *ptr;
    }

    template <auto tEnumValue>
    srefl::Property& Enum(char const* aName)
    {
      using tEnumType = decltype(tEnumValue);
      constexpr bool isSigned = std::is_signed_v<tEnumType>;

      std::unique_ptr<srefl::Function> enumGetter;

      if (isSigned)
      {
        constexpr i64 value = (i64)tEnumValue;

        enumGetter = Detail::Meta::FunctionBinding<decltype(ReturnValue<value>)>:: template BindFunction<ReturnValue<value>>(aName);
      }
      else
      {
        constexpr u64 value = (u64)tEnumValue;

        enumGetter = Detail::Meta::FunctionBinding<decltype(ReturnValue<value>)>:: template BindFunction<ReturnValue<value>>(aName);
      }

      auto property = std::make_unique<srefl::Property>(aName, std::move(enumGetter), NoSetter);

      auto ptr = TypeId<tType>()->AddProperty(std::move(property));

      return *ptr;
    }

    template <auto tFieldPointer>
    srefl::Field& Field(char const *aName, PropertyBinding aBinding)
    {
      using FieldPointerType = decltype(tFieldPointer);
      using FieldType = typename DecomposeFieldPointer<FieldPointerType>::FieldType;

      std::unique_ptr<srefl::Function> getter;
      std::unique_ptr<srefl::Function> setter;

      if (PropertyBinding::Get == aBinding || PropertyBinding::GetSet == aBinding)
      {
        getter = Detail::Meta::FunctionBinding<FieldType(tType::*)()>::BindPassedFunction("Getter", Field::Getter<FieldPointerType, tFieldPointer>);
      }

      if (PropertyBinding::Set == aBinding || PropertyBinding::GetSet == aBinding)
      {
        setter = Detail::Meta::FunctionBinding<void(tType::*)(FieldType)>::BindPassedFunction("Setter", Field::Setter<FieldPointerType, tFieldPointer>);
      }

      auto field = std::make_unique<srefl::Field>(aName, std::move(getter), std::move(setter));

      auto type = TypeId<typename DecomposeFieldPointer<FieldPointerType>::FieldType>();

      field->SetPropertyType(type);
      field->SetOffset(srefl::Field::GetOffset<FieldPointerType, tFieldPointer>());

      auto ptr = TypeId<tType>()->AddField(std::move(field));
      return *ptr;
    }

    private:

    Type* mType;
  };

  template <typename tType>
  void RegisterType()
  {
    auto type = srefl::TypeId<tType>();
    Type::AddGlobalType(type->GetName(), type);

    TypeBuilder<tType> builder;
    builder.template Function<&::srefl::TypeId<tType>>("GetStaticType");
  }
}