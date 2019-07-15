#pragma once
#include "SimpleReflection/Meta.hpp"

namespace srefl
{
  class Component;
  class Object;

  class Attribute : public Base
  {
  public:
    sreflDeclareType(Attribute)
  };

  class Serializable : public Attribute
  {
  public:
    sreflDeclareType(Serializable);
    Serializable(DocumentedObject *aObject);
  };
}



