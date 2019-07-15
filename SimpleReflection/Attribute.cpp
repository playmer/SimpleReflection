#include "SimpleReflection/Attribute.hpp"

namespace srefl
{
  sreflDefineType(Attribute)
  {
    RegisterType<Attribute>();
    TypeBuilder<Attribute> builder;
  }

  sreflDefineType(Serializable)
  {
    RegisterType<Serializable>();
    TypeBuilder<Serializable> builder;
  }

  void PropertyChecked(const char *aType, DocumentedObject *aObject)
  {
    auto property = dynamic_cast<Property*>(aObject);

    auto getter = property->GetGetter();
    auto setter = property->GetSetter();

    //DebugObjection(nullptr == getter,
    //               "%s %s missing getter",
    //               aType,
    //               property->GetName().c_str());
    //
    //DebugObjection(nullptr == setter,
    //               "%s %s missing setter",
    //               aType,
    //               property->GetName().c_str());

    auto parameters = setter->GetParameters();

    //DebugObjection(parameters.size() != 2,
    //               "%s %s must have a setter that takes only one parameter,"
    //               " an instance of an object of the same type as it's getter returns.",
    //               aType,
    //               property->GetName().c_str());
    //
    //DebugObjection(parameters[1].mType->GetMostBasicType() != getter->GetReturnType()->GetMostBasicType(),
    //               "%s %s must have a setter that takes as it's "
    //               "first parameter, the same type as it's getter returns. \n"
    //               "  Setter First Parameter Type: %s"
    //               "  Getter Return Type : %s",
    //               aType,
    //               property->GetName().c_str(),
    //               parameters[1].mType->GetName().c_str(),
    //               getter->GetName().c_str());

    UnusedArguments(aType, getter, parameters);
  }

  Serializable::Serializable(DocumentedObject *aObject)
  {
    PropertyChecked("Serializable", aObject);
  }
}

