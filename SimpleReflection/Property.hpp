#pragma once

#include "SimpleReflection/Utilities.hpp"

#include "SimpleReflection/Function.hpp"

namespace srefl
{
  // TODO: Have some requirements on the types of setters and getters.
  class Property : public DocumentedObject
  {
  public:
    sreflDeclareType(Property)

    Property(Property&) = delete;

    Property(const char *aName,
                        std::unique_ptr<Function> aGetter,
                        std::unique_ptr<Function> aSetter);

    Type* GetOwningType() { return mOwningType; }
    Type* GetPropertyType() { return mType; }
    const std::string& GetName() const { return mName; }
    Function* GetGetter() { return mGetter.get(); }
    Function* GetSetter() { return mSetter.get(); }
    void SetPropertyType(Type *aType) { mType = aType; }

  protected:
    Type *mOwningType;
    Type *mType;
    std::string mName;
    std::unique_ptr<Function> mGetter;
    std::unique_ptr<Function> mSetter;
  };
}