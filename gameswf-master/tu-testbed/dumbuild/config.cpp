// config.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include "config.h"
#include "util.h"

Config::Config() {
}

const string& Config::GetVar(const string& varname) const {
  map<string, string>::const_iterator it = vars_.find(varname);
  if (it == vars_.end()) {
    return empty_string_;
  }
  return it->second;
}

void Config::SetVar(const string& varname, const string& new_value) {
  vars_[varname] = new_value;
}

Res Config::Init(const Context* context, const string& name,
                 const Json::Value& init_object) {
  Res res = Object::Init(context, name, init_object);
  if (!res.Ok()) {
    return res;
  }

  assert(init_object.isObject());

  string null_char;
  null_char += '\0';

  // We handle some variables specially, treating them as key/value
  // pairs, with certain separators used to pack them into a string.
  struct MapVarInfo {
    const string varname;
    const string separator;
    const string separator2;
  } const var_infos[] = {
    { "compile_environment", "=", null_char },
  };

  const vector<string>& keys = init_object.keys_in_insert_order();
  for (size_t key_i = 0; key_i < keys.size(); key_i++) {
    const string& key = keys[key_i];
    assert(init_object.isMember(key));
    const Json::Value& value = init_object[key];

    if (key == "name" || key == "type") {
      // Skip these.
      continue;
    }

    bool found_mapvar = false;
    for (size_t i = 0; i < ARRAY_SIZE(var_infos); i++) {
      const MapVarInfo& mvi = var_infos[i];
      if (mvi.varname == key) {
        found_mapvar = true;
        string result;
        res = ParseValueStringOrMap(context, this, value, mvi.separator,
                                    mvi.separator2, &result);
        if (!res.Ok()) {
          return res;
        }
        if (mvi.separator2 == null_char) {
          result += mvi.separator2;
        }
        SetVar(mvi.varname, result);
        break;
      }
    }

    if (!found_mapvar) {
      // Set a normal string var.
      if (!IsValidVarname(key)) {
        return Res(ERR_PARSE, string("config defines invalid variable name '") +
                   key + "'.  Varnames must be alphanumeric, "
                   "plus - or _");
      }
      
      string result;
      res = ParseValueStringOrMap(context, this, value, "=", ";", &result);
      if (!res.Ok()) {
        return res;
      }

      if (key == "inherit") {
        // Inherit values from the named config.
        const Config* parent = context->GetNamedConfig(result.c_str());
        if (!parent) {
          return Res(ERR_PARSE, "config '" + this->name() +
                     "' can't inherit from unknown config '" +
                     result + "'");
        }
        InheritVars(parent);
      } else {
        SetVar(key, result);
      }
    }
  }

  // Pre-fill our compile, lib, & link templates.
  res = ::FillTemplate(GetVar("compile_template"), vars_, true,
                       &prefilled_compile_template_);
  if (!res.Ok()) {
    return res;
  }

  res = ::FillTemplate(GetVar("lib_template"), vars_, true,
                       &prefilled_lib_template_);
  if (!res.Ok()) {
    return res;
  }

  res = ::FillTemplate(GetVar("link_template"), vars_, true,
                       &prefilled_link_template_);
  if (!res.Ok()) {
    return res;
  }

  return res;
}

void Config::InsertVarsIntoMap(map<string, string>* out) const {
  for (map<string, string>::const_iterator it = vars_.begin();
       it != vars_.end();
       ++it) {
    (*out)[it->first] = it->second;
  }
}

// Copy all variables from *parent.
void Config::InheritVars(const Config* parent) {
  for (map<string, string>::const_iterator it = parent->vars_.begin();
       it != parent->vars_.end();
       ++it) {
    SetVar(it->first, it->second);
  }
}

Res Config::FillTemplate(const string& template_string, string* out) const {
  return ::FillTemplate(template_string, vars_, false, out);
}
