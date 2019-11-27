#pragma once
#include "types.hpp"

class goapRootReality {
  /* This stores values in an array rather than mapping;
   * Faster and more memory-efficient.
   */
};

class goapReality {
public:
  class varProxy {
  public:
    varProxy(goapReality &reality, const enumType &name)
        : name(name), reality(reality) {}

    const enumType &name;

    numType getValue() const;
    void setValue(numType &num);
    void operator=(numType &num);
    void operator+=(numType &num);
    void operator-=(numType &num);

  protected:
    goapReality &reality;
  }; // varProxy is returned in place of actual values.

  goapReality(realityVarMap &_variables); // Root reality
  goapReality(const goapReality &parent,  // Proxy + initial values
              realityVarMap &_variables);

  const numType &getVariable(enumType name) const;
  void setVariable(const enumType &name, numType &value);
  void updateVariables(realityVarMap &variables);

  // Read-only references to private variables;
  const goapReality &parent() const { return _parent; }
  const realityVarMap &variables() const { return _variables; }

  varProxy operator[](const enumType &name);
  numType operator[](const enumType &name) const;

protected:
  const goapReality &_parent = *this; // Const ref to parent or itself.
  realityVarMap &_variables;          // Ref to reality variable mapping

  numType &doLookup(const enumType &name) const;
};
