#include "reality.hpp"

// varProxy
numType goapReality::varProxy::getValue() const {
  return reality.doLookup(name);
}

void goapReality::varProxy::setValue(numType &num) {
  reality.setVariable(name, num);
}

void goapReality::varProxy::operator=(numType &num) {
  reality.setVariable(name, num);
}

void goapReality::varProxy::operator+=(numType &num) {
  numType tot = reality.getVariable(name) + num;
  reality.setVariable(name, tot);
}

goapReality::varProxy goapReality::operator[](const enumType &name) {
  return varProxy(*this, name);
}

numType goapReality::operator[](const enumType &name) const {
  return this->getVariable(name);
}

// goapReality

goapReality::goapReality(realityVarMap &_variables) : _variables(_variables) {}

goapReality::goapReality(const goapReality &parent, realityVarMap &_variables)
    : _parent(parent), _variables(_variables) {}

const numType &goapReality::getVariable(enumType name) const {
  return doLookup(name);
}

void goapReality::setVariable(const enumType &name, numType &value) {
  _variables[name] = value; // Set in this reality.
}

void goapReality::updateVariables(realityVarMap &variables) {
  for (const auto &v : variables)
    _variables[v.first] = v.second;
}

numType &goapReality::doLookup(const enumType &name) const {
  auto iter = _variables.find(name); // Lookup
  if (iter != _variables.end())      // >Found it
    return iter->second;             // >>Return value
  if (&_parent != this)              // Has a parent
    return _parent.doLookup(name);   // >return (Ask parent)
  throw std::out_of_range("Key does not exist");
}
