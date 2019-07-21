
#include "SimpleReflection/Meta.hpp"
#include "SimpleReflection/Type.hpp"

namespace srefl
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  sreflDefineType(DocumentedObject)
  {
    RegisterType<DocumentedObject>();
    TypeBuilder<DocumentedObject> builder;
    builder.Property<&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation>("Documentation");
  }

  sreflDefineType(Type)
  {
    RegisterType<Type>();
    TypeBuilder<Type> builder;

    builder.Function<&Type::GetGlobalType>("GetGlobalType")
      .SetParameterNames("aName");

    builder.Property<&Type::Name, NoSetter>("Name")
      .SetDocumentation("Name of the Type.");
    builder.Property<&Type::Hash, NoSetter>("Hash")
      .SetDocumentation("Hash of the Type.");
    builder.Property<&Type::GetAllocatedSize, NoSetter>("AllocatedSize")
      .SetDocumentation("Allocated size of the Type.");
    builder.Property<&Type::GetStoredSize, NoSetter>("StoredSize")
      .SetDocumentation("Stored size of the Type.");
    builder.Property<&Type::GetUnqualifiedSize, NoSetter>("UnqualifiedSize")
      .SetDocumentation("Unqualified size of the Type.");
  }

  inline Type::~Type()
  {

  }

  Function* Type::AddFunction(std::unique_ptr<Function> aFunction)
  {
    auto function = mFunctions.Emplace(aFunction->GetName(), std::move(aFunction));
    return function->second.get();
  }

  Property* Type::AddProperty(std::unique_ptr<Property> aProperty)
  {
    auto property = mProperties.Emplace(aProperty->GetName(), std::move(aProperty));
    return property->second.get();
  }

  Field* Type::AddField(std::unique_ptr<Field> aField)
  {
    auto field = mFields.Emplace(aField->GetName(), std::move(aField));
    return static_cast<Field*>(field->second.get());
  }

  bool Type::IsA(Type *aType, Type *aTypeToStopAt)
  {
    //DebugAssert(IsA(aTypeToStopAt),
    //            "The aTypeToStopAt must be a base type of the type being searched.");

    Type *base = this;

    while (base && base != aTypeToStopAt)
    {
      if (base == aType)
      {
        return true;
      }

      base = base->GetBaseType();
    }

    return false;
  }

  bool Type::IsA(Type *aType)
  {
    Type *base = this;

    if (base == aType)
    {
      return true;
    }

    while (base->GetBaseType())
    {
      base = base->GetBaseType();

      if (base == aType)
      {
        return true;
      }
    }

    return false;
  }

  Type* Type::GetMostBasicType()
  {
    Type *type = this;

    while (type->GetPointerTo()   ||
           type->GetReferenceTo() ||
           type->GetConstOf())
    {
      if (type->GetPointerTo())
      {
        type = type->GetPointerTo();
      }
      else if (type->GetReferenceTo())
      {
        type = type->GetReferenceTo();
      }
      else if (type->GetConstOf())
      {
        type = type->GetConstOf();
      }
    }

    return type;
  }

  Property* Type::GetFirstField(const char *aName)
  {
    std::string name{ aName };

    auto it = mFields.FindFirst(name);

    if (it != mFields.end())
    {
      return it->second.get();
    }

    return nullptr;
  }

  void Type::AddGlobalType(const std::string &aName, Type *aType)
  {
    auto it = sGlobalTypes.find(aName);

    if (it != sGlobalTypes.end())
    {
      std::cout << "Type of the name " << aName << " already exists, not adding." << std::endl;
      return;
    }

    sGlobalTypes.emplace(aName, aType);
  }

  Type* Type::GetGlobalType(const std::string &aName)
  {
    auto it = sGlobalTypes.find(aName);

    Type *toReturn{ nullptr };

    if (it != sGlobalTypes.end())
    {
      toReturn = it->second;
    }

    if (toReturn == nullptr)
    {
      printf("Could not find a type named %s, did you rename/misspell it/forget to Define/InitializeType it?\n", aName.c_str());
    }

    return toReturn;
  }

  sreflDefineType(Property)
  {
    RegisterType<Property>();
    TypeBuilder<Property> builder;

    builder.Property<&Property::GetOwningType, NoSetter>("OwningType")
      .SetDocumentation("Type that owns this Property.");
    builder.Property<&Property::GetPropertyType, NoSetter>("PropertyType")
      .SetDocumentation("Type of the Property, what we can get or set.");
    builder.Property<&Property::GetName, NoSetter>("Name")
      .SetDocumentation("Name of the Property.");
    builder.Property<&Property::GetGetter, NoSetter>("Getter")
      .SetDocumentation("Getter function of the Property, may be null.");
    builder.Property<&Property::GetSetter, NoSetter>("Setter")
      .SetDocumentation("Setter function of the Property, may be null.");
  }

  Property::Property(const char *aName,
                     std::unique_ptr<Function> aGetter,
                     std::unique_ptr<Function> aSetter)
    : mName(aName)
    , mGetter(std::move(aGetter))
    , mSetter(std::move(aSetter))
  {
    //DebugAssert((nullptr != mGetter) || (nullptr != mSetter),
    //            "At least one of the getter and setter must be set.");

    if (mGetter)
    {
      mOwningType = mGetter->GetOwningType();
      mType = mGetter->GetReturnType()->GetMostBasicType();
    }

    if (mSetter)
    {
      mOwningType = mSetter->GetOwningType();
      auto parameters = mSetter->GetParameters();

      //DebugObjection(parameters.size() != 2,
      //               "%s %s must have a setter that takes only one parameter,"
      //               " an instance of an object of the same type as it's getter returns.",
      //               mOwningType->GetName().c_str(),
      //               mName.c_str());

      mType = parameters[1].mType->GetMostBasicType();
    }

    // TODO: This check doesn't work as intended, should be something more like, convertible to.
    //if (mSetter && mGetter)
    //{
    //  auto parameters = mSetter->GetParameters();
    //
    //  DebugObjection(parameters[1].mType->GetMostBasicType() != mGetter->GetReturnType()->GetMostBasicType(),
    //                 "%s %s must have a setter that takes as it's "
    //                 "first parameter, the same type as it's getter returns. \n"
    //                 "  Setter First Parameter Type: %s"
    //                 "  Getter Return Type : %s",
    //                 mOwningType->GetName().c_str(),
    //                 mName.c_str(),
    //                 parameters[1].mType->GetName().c_str(),
    //                 mGetter->GetName().c_str());
    //}
  }

  sreflDefineType(Field)
  {
    RegisterType<Field>();
    TypeBuilder<Field> builder;
  }

  void InitializeReflection()
  {
    InitializeType<void>();
    InitializeType<bool>();
    InitializeType<srefl::s8>();
    InitializeType<srefl::i8>();
    InitializeType<srefl::i16>();
    InitializeType<srefl::i32>();
    InitializeType<srefl::i64>();
    InitializeType<srefl::u8>();
    InitializeType<srefl::u16>();
    InitializeType<srefl::u32>();
    InitializeType<srefl::u64>();
    InitializeType<float>();
    InitializeType<double>();
    InitializeType<std::string>();
    InitializeType<Attribute>();
    InitializeType<Serializable>();
    InitializeType<Field>();
    InitializeType<Function>();
    InitializeType<DocumentedObject>();
    InitializeType<Type>();
  }
}


sreflDefineExternalType(srefl::s8)
{
  RegisterType<srefl::s8>();
  TypeBuilder<srefl::s8> builder;
}

sreflDefineExternalType(srefl::i8)
{
  RegisterType<srefl::i8>();
  TypeBuilder<srefl::i8> builder;
}

sreflDefineExternalType(srefl::i16)
{
  RegisterType<srefl::i16>();
  TypeBuilder<srefl::i16> builder;
}

sreflDefineExternalType(srefl::i32)
{
  RegisterType<srefl::i32>();
  TypeBuilder<srefl::i32> builder;
}

sreflDefineExternalType(srefl::i64)
{
  RegisterType<srefl::i64>();
  TypeBuilder<srefl::i64> builder;
}

sreflDefineExternalType(srefl::u8)
{
  RegisterType<srefl::u8>();
  TypeBuilder<srefl::u8> builder;
}

sreflDefineExternalType(srefl::u16)
{
  RegisterType<srefl::u16>();
  TypeBuilder<srefl::u16> builder;
}

sreflDefineExternalType(srefl::u32)
{
  RegisterType<srefl::u32>();
  TypeBuilder<srefl::u32> builder;
}

sreflDefineExternalType(srefl::u64)
{
  RegisterType<srefl::u64>();
  TypeBuilder<srefl::u64> builder;
}

sreflDefineExternalType(void)
{
  RegisterType<void>();
  TypeBuilder<void> builder;
}

sreflDefineExternalType(bool)
{
  RegisterType<bool>();
  TypeBuilder<bool> builder;
}

sreflDefineExternalType(float)
{
  RegisterType<float>();
  TypeBuilder<float> builder;
}

sreflDefineExternalType(double)
{
  RegisterType<double>();
  TypeBuilder<double> builder;
}

sreflDefineExternalType(std::string)
{
  RegisterType<std::string>();
  TypeBuilder<std::string> builder;
}
