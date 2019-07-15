add_library(SimpleReflection)

target_include_directories(SimpleReflection PUBLIC ${simpleReflectionDirectory})

target_compile_features(SimpleReflection PUBLIC cxx_std_17)

target_sources(SimpleReflection
PRIVATE
    Any.hpp
    Attribute.cpp
    Attribute.hpp
    ConstexprString.hpp
    Field.hpp
    ForwardDeclarations.hpp
    Function.cpp
    Function.hpp
    Iterator.hpp
    Meta.hpp
    OrderedMap.hpp
    OrderedMultiMap.hpp
    Property.hpp
    Range.hpp
    Reflection.hpp
    Type.cpp
    Type.hpp
    TypeTraits.hpp
    Utilities.hpp
)


