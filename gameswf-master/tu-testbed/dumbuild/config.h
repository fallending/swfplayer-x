// config.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// The Config type holds compiler command-line templates and flags,
// etc.

#ifndef CONFIG_H_
#define CONFIG_H_

#include "dmb_types.h"
#include "object.h"

class Config : public Object {
 public:
  Config();

  virtual Res Init(const Context* context,
		   const string& name,
		   const Json::Value& value);

  virtual Config* CastToConfig() {
    return this;
  }

  const string& GetVar(const string& varname) const;
  void SetVar(const string& varname, const string& value);

  // Some important vars.
  const string& prefilled_compile_template() const {
    return prefilled_compile_template_;
  }
  const string& prefilled_lib_template() const {
    return prefilled_lib_template_;
  }
  const string& prefilled_link_template() const {
    return prefilled_link_template_;
  }
  const string& compile_environment() const {
    return GetVar("compile_environment");
  }
  const string& obj_extension() const {
    return GetVar("obj_extension");
  }
  const string& lib_extension() const {
    return GetVar("lib_extension");
  }
  const string& exe_extension() const {
    return GetVar("exe_extension");
  }

  // Fill the given template using values assigned to our variables.
  // Result goes into *out.
  Res FillTemplate(const string& template_string, string* out) const;

  // Copy all our vars into the given map.
  void InsertVarsIntoMap(map<string, string>* out) const;

 private:
  void InheritVars(const Config* parent);
  
  string empty_string_;
  map<string, string> vars_;
  string prefilled_compile_template_;
  string prefilled_lib_template_;
  string prefilled_link_template_;
};

#endif  // CONFIG_H_
