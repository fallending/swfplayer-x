// eval.cpp  -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// This is a very dumb "Lisp-like" interpreter.  This is barely enough
// power to do some autoconfiguration and simplification in the build
// files.
//
// Because this is dumbuild, I don't apologize for how crippled it is.
// Note:
//
//   * Not Turing-complete.  There's no way to loop or recurse, or
//     even define a function.
//
//   * Uses json syntax.  Pretty unwieldy, but it's consistent w/
//     dumbuild.
//
//   * What variables exist are key/value string pairs in a config
//     object.
//
//   * No garbage collection.  No need for it :)
//
//   * Has basic conditionals 'if' and 'case'.
//
//   * Has a 'fill' function for string template replacement.
//
//   * Has some other really simple stuff like 'file_exists' and
//     'dir_exists'.

#include <stdio.h>
#include "eval.h"
#include "config.h"
#include "os.h"

class LispValue;
class LispContext;
typedef Res (*LispFunction)(const LispContext* ctx, const LispValue& args,
                            LispValue* result);

// Dumbuild-specific environment info used for lisp eval.  References
// a Context and optionally a Config.  Maybe more stuff in the future.
class LispContext {
 public:
  LispContext(const Context* ctx, Config* cfg) : ctx_(ctx), cfg_(cfg) {
    assert(ctx);
  }

  const Context* ctx() const {
    return ctx_;
  }

  Config* cfg() const {
    return cfg_;
  }

 private:
  const Context* ctx_;
  Config* cfg_;
};

class LispValue {
 public:
  enum Type {
    NIL,
    STRING,
    ARRAY,
    FUNCTION,
    CONFIG,
  };

  LispValue() : type_(NIL) {
  }
  explicit LispValue(const string& str) : type_(STRING), string_val_(str) {
  }
  explicit LispValue(Config* c) : type_(CONFIG), config_val_(c) {
  }
  explicit LispValue(LispFunction fn) : type_(FUNCTION), function_val_(fn) {
  }
  explicit LispValue(const Json::Value& val) {
    if (val.isArray()) {
      type_ = ARRAY;
      array_val_ = &val;
    } else if (val.isString()) {
      type_ = STRING;
      string_val_ = val.asString();
    } else {
      // This is really an error -- perhaps we need a better way to
      // indicate it.
      type_ = NIL;
    }
  }
    
  Type type() const {
    return type_;
  }

  const std::string& string_val() const {
    assert(type_ == STRING);
    return string_val_;
  }

  LispFunction function_val() const {
    assert(type_ == FUNCTION);
    return function_val_;
  }

  const Json::Value& array_val() const {
    assert(type_ == ARRAY);
    return *array_val_;
  }

  Config* config_val() const {
    assert(type_ == CONFIG);
    return config_val_;
  }

  bool operator==(const LispValue& v) const {
    if (type() != v.type()) {
      return false;
    }
    switch (type()) {
      case NIL:
        return true;
      case STRING:
        return string_val() == v.string_val();
      case ARRAY:
        return array_val() == v.array_val();
      case FUNCTION:
        return function_val() == v.function_val();
      case CONFIG:
        return config_val() == v.config_val();
    }
    return false;
  }

  std::string ToStyledString() const {
    return "TODO: LispValue::ToStyledString()";
  }

 private:
  Type type_;
  string string_val_;
  LispFunction function_val_;
  Config* config_val_;
  const Json::Value* array_val_;
};

static std::map<string, LispFunction> s_functions;
static bool s_inited = false;

Res Eval(const LispContext* lc, const LispValue& val, LispValue* result);
Res EvalToString(const LispContext* lc, const Json::Value& val, string* result);

// First arg is the name of a config.  This function returns the named
// config if it exists; otherwise it returns NIL.
Res ConfigFunc(const LispContext* lc, const LispValue& args, LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "'config' function takes one string argument.  "
               "Was given: '" + args.ToStyledString() + "'");
  }

  string arg_val;
  Res err = EvalToString(lc, args.array_val()[1u], &arg_val);
  if (!err.Ok()) {
    return err;
  }

  Config* conf = lc->ctx()->GetNamedConfig(arg_val.c_str());
  if (conf) {
    *result = LispValue(conf);
  }
  return Res(OK);
}

// Takes a list of config names.  Returns the first existing config
// whose 'detected' field is true, or NIL if no config is detected.
Res ConfigDetectedFunc(const LispContext* lc, const LispValue& args,
                       LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  // Test each arg.
  for (unsigned int i = 1; i < args.array_val().size(); i++) {
    const Json::Value& this_arg = args.array_val()[i];
    string arg_val;
    Res err = EvalToString(lc, this_arg, &arg_val);
    if (!err.Ok()) {
      return err;
    }

    Config* conf = lc->ctx()->GetNamedConfig(arg_val.c_str());
    if (conf) {
      const string& detected = conf->GetVar("detected");
      if (detected != "" &&  detected != "[]") {
        *result = LispValue(conf);
        return Res(OK);
      }
    }
  }

  // No match.
  return Res(OK);
}

// Takes one arg, a pathname of a dir.  Returns true if the dir
// exists, or NIL if it does not.
Res DirExistsFunc(const LispContext* lc, const LispValue& args,
                  LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "dir_exists takes exactly one arg.  Was given: '" +
               args.array_val().toStyledString() + "'");
  }

  string arg_val;
  Res err = EvalToString(lc, args.array_val()[1u], &arg_val);
  if (!err.Ok()) {
    return err;
  }

  if (DirExists(arg_val)) {
    *result = LispValue(string("exists"));
  }

  return Res(OK);
}

// Takes one arg, a pathname of a file.  Returns true if the file
// exists and is executable.  Returns NIL otherwise.
Res ExeExistsFunc(const LispContext* lc, const LispValue& args,
                  LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "exe_exists takes exactly one arg.  Was given: '" +
               args.array_val().toStyledString() + "'");
  }

  string arg_val;
  Res err = EvalToString(lc, args.array_val()[1u], &arg_val);
  if (!err.Ok()) {
    return err;
  }

  if (ExeExists(arg_val)) {
    *result = LispValue(string("exists"));
  }

  return Res(OK);
}

// Takes one arg, a pathname of a file.  Returns true if the file
// exists.  Returns NIL otherwise.
Res FileExistsFunc(const LispContext* lc, const LispValue& args,
                   LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "exe_exists takes exactly one arg.  Was given: '" +
               args.array_val().toStyledString() + "'");
  }

  string arg_val;
  Res err = EvalToString(lc, args.array_val()[1u], &arg_val);
  if (!err.Ok()) {
    return err;
  }

  if (FileExists(arg_val)) {
    *result = LispValue(string("exists"));
  }

  return Res(OK);
}

Res FillFunc(const LispContext* lc, const LispValue& args,
             LispValue* result) {
  if (!lc->cfg()) {
    return Res(ERR_EVAL, "'fill' called with no config.");
  }

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "'fill' takes exactly one arg.  Was given: '" +
               args.array_val().toStyledString() + "'");
  }
  
  string arg_val;
  Res err = EvalToString(lc, args.array_val()[1u], &arg_val);
  if (!err.Ok()) {
    return err;
  }

  string filled;
  err = lc->cfg()->FillTemplate(arg_val, &filled);
  if (!err.Ok()) {
    return err;
  }

  *result = LispValue(filled);
  return Res(OK);
}

// Returns the value of the named command-line flag, if any.
// If there's no matching flag, returns NIL.
Res FlagFunc(const LispContext* lc, const LispValue& args, LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "flag takes exactly one arg.  Was given: '" +
               args.array_val().toStyledString() + "'");
  }

  string arg_val;
  Res err = EvalToString(lc, args.array_val()[1u], &arg_val);
  if (!err.Ok()) {
    return err;
  }

  if (lc->ctx()->HasArg(arg_val.c_str())) {
    *result = LispValue(lc->ctx()->GetArgValue(arg_val.c_str()));
  }

  return Res(OK);
}

// Takes a project path and makes it absolute.
//
// ["abs_path", "../some/path"]
// ["abs_path", "#../some/path"]
// etc
Res AbsPathFunc(const LispContext* lc, const LispValue& args,
                LispValue* result) {
  assert(args.type() == LispValue::ARRAY);

  *result = LispValue();  // default result is NIL.

  if (args.array_val().size() != 2) {
    return Res(ERR_EVAL, "AbsPathFunc: needs exactly one arg");
  }

  string argval;
  Res err = EvalToString(lc, args.array_val()[1u], &argval);
  if (!err.Ok()) {
    return err;
  }

  // TODO: need base_dir here!!!!
  string canonical = Canonicalize("" /*xxx*/, argval);
  *result = LispValue(PathJoin(lc->ctx()->tree_root(), canonical));

  return Res(OK);
}

// Takes a string delimiter and joins the rest of the args together
// with the delimiter.
//
// ["join", ",", "a", "b", "c"]  -->  "a,b,c"
// etc
Res JoinFunc(const LispContext* lc, const LispValue& args,
             LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();  // default result is NIL.

  if (args.array_val().size() < 2) {
    return Res(ERR_EVAL, "JoinFunc: needs at least one arg");
  }

  string delimiter;
  Res err = EvalToString(lc, args.array_val()[1u], &delimiter);
  if (!err.Ok()) {
    return err;
  }

  string str_result;
  for (unsigned int i = 2; i < args.array_val().size(); i++) {
    const Json::Value& this_arg = args.array_val()[i];
    string this_str;
    err = EvalToString(lc, this_arg, &this_str);
    if (!err.Ok()) {
      return err;
    }

    if (i > 2) {
      str_result += delimiter;
    }
    str_result += this_str;
  }
  *result = LispValue(str_result);

  return Res(OK);
}

// Returns the first result that has a key that equals test_expr.
// Returns NIL if there's no match.
//
// ["case", test_expr,
//   [[val0a, val0b, ...], result0],
//   [[val1, ...], result1],
//   [[val2, ...], result2],
//   ...
// ]
//
// For example:
// ["case", ["flag", "mode"],
//   [["debug"], "-Od"],
//   [["release", "profile], "-Ox"]]
//
// Functions like a C++ if/else-if/else-if chain.
Res CaseFunc(const LispContext* lc, const LispValue& args, LispValue* result) {
  assert(args.type() == LispValue::ARRAY);

  *result = LispValue();  // default result is NIL.

  if (args.array_val().size() < 3) {
    return Res(ERR_EVAL, "CaseFunc: needs at least two args: a test expression "
               "and one test pair: '" +
               args.array_val().toStyledString() + "'");
  }
  LispValue predicate_result;
  Res err = Eval(lc, LispValue(args.array_val()[1u]), &predicate_result);
  if (!err.Ok()) {
    return err;
  }

  // Test each pair in turn.
  for (unsigned int i = 2; i < args.array_val().size(); i++) {
    const Json::Value& this_pair = args.array_val()[i];
    if (!this_pair.isArray() || this_pair.size() != 2) {
      return Res(ERR_EVAL, StringPrintf("CaseFunc: arg %u is not a pair.", i) +
                 " in expr: '" + args.ToStyledString() + "'");
    }
    const Json::Value& key_array = this_pair[0u];
    if (!key_array.isArray()) {
      return Res(ERR_EVAL, StringPrintf("CaseFunc: key array in arg %u is not "
                                        "an array.", i) + " in expr: '" +
                 args.ToStyledString() + "'");
    }
    for (unsigned int j = 0; j < key_array.size(); j++) {
      LispValue key_result;
      err = Eval(lc, LispValue(key_array[j]), &key_result);
      if (!err.Ok()) {
        return err;
      }
      if (key_result == predicate_result) {
        // Match.  Return the result of evaluating the second element of
        // the pair.
        return Eval(lc, LispValue(this_pair[1u]), result);
      }
    }
  }
  return Res(OK);
}

// Evaluates a predicate and then, depending on the result, returns
// the result of either the first or (optional) second clauses.
Res IfFunc(const LispContext* lc, const LispValue& args, LispValue* result) {
  assert(args.type() == LispValue::ARRAY);
  *result = LispValue();

  if (args.array_val().size() < 3) {
    return Res(ERR_EVAL, "IfFunc: needs at least two args: a predicate and "
               "a true result: '" + args.array_val().toStyledString() + "'");
  }
  LispValue predicate(args.array_val()[1u]);
  LispValue true_expr(args.array_val()[2u]);
  LispValue predicate_result;
  Res err = Eval(lc, predicate, &predicate_result);
  if (!err.Ok()) {
    return err;
  }
    
  if (predicate_result.type() != LispValue::NIL) {
    // True.
    return Eval(lc, true_expr, result);
  } else {
    // False.
    if (args.array_val().size() >= 4) {
      LispValue false_expr(args.array_val()[3u]);
      return Eval(lc, false_expr, result);
    }
    // else return default value of NIL
  }
  return Res(OK);
}

Res OrFunc(const LispContext* lc, const LispValue& args, LispValue* result) {
  assert(args.type() == LispValue::ARRAY);

  *result = LispValue();  // default to NIL
  for (size_t i = 1; i < args.array_val().size(); i++) {
    LispValue val(args.array_val()[static_cast<unsigned>(i)]);
    Res err = Eval(lc, val, result);
    if (!err.Ok()) {
      return err;
    }

    if (result->type() != LispValue::NIL) {
      // First true arg.
      return Res(OK);
    }
    // else try the next arg.
  }

  // Return the value of the last arg.
  return Res(OK);
}

Res AndFunc(const LispContext* lc, const LispValue& args, LispValue* result) {
  assert(args.type() == LispValue::ARRAY);

  *result = LispValue();  // default to NIL
  for (size_t i = 1; i < args.array_val().size(); i++) {
    LispValue val(args.array_val()[static_cast<unsigned>(i)]);
    Res err = Eval(lc, val, result);
    if (!err.Ok()) {
      return err;
    }

    if (result->type() == LispValue::NIL) {
      // First false arg.
      return Res(OK);
    }
    // else try the next arg.
  }

  // Return the value of the last arg tested.
  return Res(OK);
}

void InitEval() {
  s_inited = true;
  s_functions["abs_path"] = AbsPathFunc;
  s_functions["case"] = CaseFunc;
  s_functions["config"] = ConfigFunc;
  s_functions["config_detected"] = ConfigDetectedFunc;
  s_functions["dir_exists"] = DirExistsFunc;
  s_functions["exe_exists"] = ExeExistsFunc;
  s_functions["file_exists"] = FileExistsFunc;
  s_functions["fill"] = FillFunc;
  s_functions["flag"] = FlagFunc;
  s_functions["if"] = IfFunc;
  s_functions["join"] = JoinFunc;
  s_functions["or"] = OrFunc;
  s_functions["and"] = AndFunc;
}

Res EvalToString(const Context* ctx, Config* cfg, const Json::Value& val,
                 string* out) {
  LispContext lc(ctx, cfg);
  return EvalToString(&lc, val, out);
}

// Internal implementation.
Res EvalToString(const LispContext* lc, const Json::Value& val,
                 string* out) {
  Res err;
  LispValue lisp_val(val);
  LispValue result;
  err = Eval(lc, lisp_val, &result);
  if (!err.Ok()) {
    return err;
  }

  if (result.type() == LispValue::NIL) {
    *out = "[]";
    return Res(OK);
  }
  if (result.type() == LispValue::STRING) {
    *out = result.string_val();
    return Res(OK);
  }
  if (result.type() == LispValue::CONFIG) {
    *out = result.config_val()->name();
    return Res(OK);
  }
  return Res(ERR_EVAL, "EvalToString(): result of '" + val.toStyledString() +
             "' is not convertible to string.\nResult is: '" +
             result.ToStyledString() + "'");
}

Res EvalToFunction(const LispContext* lc, const Json::Value& expr,
                   LispFunction* result) {
  assert(s_inited);
  LispValue lisp_expr(expr);
  LispValue subresult;
  Res err = Eval(lc, lisp_expr, &subresult);
  if (!err.Ok()) {
    return err;
  }
  if (subresult.type() == LispValue::FUNCTION) {
    *result = subresult.function_val();
    return Res(OK);
  }
  if (subresult.type() == LispValue::STRING) {
    // Look up function by name.
    std::map<string, LispFunction>::const_iterator it =
      s_functions.find(subresult.string_val());
    if (it == s_functions.end()) {
      return Res(ERR_EVAL, "EvalToFunction() can't find function named '" +
                 subresult.string_val() + "'");
    }
    *result = it->second;
    return Res(OK);
  }

  return Res(ERR_EVAL, "Invalid type passed to EvalToFunction(): '" +
             expr.toStyledString() + "' -- should eval to a function.");
}

Res EvalArray(const LispContext* lc, const LispValue& val, LispValue* result) {
  assert(val.type() == LispValue::ARRAY);

  if (val.array_val().size() == 0) {
    // Empty array evaluate to NIL.
    *result = LispValue();
    return Res(OK);
  }

  LispFunction function;
  Json::Value head = val.array_val().get(0u, Json::Value::null);
  Res err = EvalToFunction(lc, head, &function);
  if (!err.Ok()) {
    err.AppendDetail("\nIn EvalToString() evaluating head of: '" +
                     val.ToStyledString() + "'");
    return err;
  }
  assert(function != NULL);

  // Unlike normal Scheme eval, we don't automatically evaluate the
  // function args.  This is because we use "functions" to implement
  // syntax, like "if", etc.  So functions are responsible for
  // evaluating their own args.

  // Invoke the function.
  err = (*function)(lc, val, result);
  if (!err.Ok()) {
    err.AppendDetail("\nIn EvalToString() evaluating function at head of: '" +
                     val.ToStyledString() + "'");
  }
  return err;
}

Res Eval(const LispContext* lc, const LispValue& val, LispValue* result) {
  if (val.type() == LispValue::STRING) {
    // Evals to itself.
    *result = val;
    return Res(OK);
  }
  if (val.type() == LispValue::ARRAY) {
    return EvalArray(lc, val, result);
  }
  return Res(ERR_EVAL, "Unexpected type in Eval(), val = '" +
             val.ToStyledString() + "'");
}

// Tests -----------------------------------------------------------------

void TestLispValue() {
  printf("TestLispValue()\n");
  LispValue nil;
  assert(nil.type() == LispValue::NIL);

  LispValue str(string("test string"));
  assert(str.type() == LispValue::STRING);
  assert(str.string_val() == "test string");

  Json::Value jarr(Json::arrayValue);
  jarr.append(Json::Value("test string"));
  jarr.append(Json::Value("string2"));
  LispValue arr(jarr);
  assert(arr.type() == LispValue::ARRAY);
  assert(arr.array_val().size() == 2);
  assert(arr.array_val()[0u].asString() == "test string");
  assert(arr.array_val()[1u].asString() == "string2");

  LispValue conf((Config*) 0);
  assert(conf.type() == LispValue::CONFIG);

  LispValue func(CaseFunc);
  assert(func.type() == LispValue::FUNCTION);
  assert(func.function_val() == CaseFunc);
}

string TestHelper(const char* expr) {
  Context ctx;
  Json::Reader reader;
  Json::Value val;
  Res err;
  string result;
  LispContext lc(&ctx, NULL);

  assert(reader.parse(expr, val));
  err = EvalToString(&lc, val, &result);
  assert(err.Ok());
  return result;
}

// Useful for checking failure cases.
Res TestHelperReturnRes(const char* expr) {
  Context ctx;
  Json::Reader reader;
  Json::Value val;
  Res err;
  string result;
  LispContext lc(&ctx, NULL);

  assert(reader.parse(expr, val));
  err = EvalToString(&lc, val, &result);
  return err;
}

void TestEvalToString() {
  printf("TestEvalToString()\n");

  // String literal.
  assert(TestHelper("\"test string\"") == "test string");

  // 'if' operator.
  assert(TestHelper("[\"if\", \"1\", \"a\", \"b\"]") == "a");
  assert(TestHelperReturnRes("[\"if\", []]").Ok() == false);
  assert(TestHelper("[\"if\", \"a\", \"1\", \"2\"]]") == "1");
  assert(TestHelper("[\"if\", [], \"1\", \"2\"]") == "2");
  assert(TestHelper("[\"if\", [], \"1\"]") == "[]");

  // 'or' operator.
  assert(TestHelper("[\"or\"]") == "[]");
  assert(TestHelper("[\"or\", \"1\"]") == "1");
  assert(TestHelper("[\"or\", [], \"1\"]") == "1");
  assert(TestHelper("[\"or\", \"1\", \"2\"]") == "1");
  assert(TestHelper("[\"or\", [], [], \"1\"]") == "1");

  // 'and' operator.
  assert(TestHelper("[\"and\"]") == "[]");
  assert(TestHelper("[\"and\", \"1\"]") == "1");
  assert(TestHelper("[\"and\", [], \"1\"]") == "[]");
  assert(TestHelper("[\"and\", \"1\", \"2\"]") == "2");
  assert(TestHelper("[\"and\", \"1\", \"2\", []]") == "[]");
  assert(TestHelper("[\"and\", \"1\", \"2\", [], \"3\"]") == "[]");

  // 'case' operator.
  assert(TestHelperReturnRes("[\"case\"]").Ok() == false);
  assert(TestHelperReturnRes("[\"case\", \"1\"]").Ok() == false);
  assert(TestHelper("[\"case\", \"miss\", [[\"debug\"], \"-Od\"]]")
         == "[]");
  assert(TestHelper("[\"case\", \"debug\", [[\"debug\"], \"-Od\"]]") == "-Od");
  assert(TestHelper("[\"case\", \"release\", [[\"debug\"], \"-Od\"],"
                    "[[\"profile\", \"release\"], \"-O2\"]]") == "-O2");
  assert(TestHelper("[\"case\", \"profile\", [[\"debug\"], \"-Od\"],"
                    "[[\"profile\", \"release\"], \"-O2\"]]") == "-O2");

  // 'dir_exists'
#ifdef _WIN32
  assert(TestHelper("[\"dir_exists\", \"c:/windows\"]") != "[]");
  assert(TestHelper("[\"dir_exists\", \"c:/veryunlikelydirname0239840237432\"]")
         == "[]");
#else  // not _WIN32
  assert(TestHelper("[\"dir_exists\", \"/usr\"]") != "[]");
  assert(TestHelper("[\"dir_exists\", \"/veryunlikelydirname0239840237432\"]")
         == "[]");
#endif  // not _WIN32

  // 'exe_exists'
#ifdef _WIN32
  assert(TestHelper("[\"exe_exists\", \"c:/windows/notepad.exe\"]") != "[]");
  assert(TestHelper("[\"exe_exists\", "
                    "\"c:/veryunlikelyexename0239840237432.exe\"]")
         == "[]");
#else  // not _WIN32
  assert(TestHelper("[\"exe_exists\", \"/bin/sh\"]") != "[]");
  assert(TestHelper("[\"exe_exists\", "
                    "\"/bin/veryunlikelyexename0239840237432\"]")
         == "[]");
#endif  // not _WIN32

  // 'file_exists'
#ifdef _WIN32
  assert(TestHelper("[\"file_exists\", \"c:/windows/system.ini\"]") != "[]");
  assert(TestHelper("[\"file_exists\", "
                    "\"c:/veryunlikelyfilename0239840237432.txt\"]")
         == "[]");
#else  // not _WIN32
  assert(TestHelper("[\"file_exists\", \"/etc/passwd\"]") != "[]");
  assert(TestHelper("[\"file_exists\", "
                    "\"/etc/veryunlikelyfilename0239840237432\"]")
         == "[]");
#endif  // not _WIN32

  // 'join'
  assert(TestHelper("[\"join\", \":\", \"a\", \"b\"]") == "a:b");
  assert(TestHelper("[\"join\", \", \", \"a\", \"1\", \"2\"]]") == "a, 1, 2");
  assert(TestHelper("[\"join\", \", \"]") == "");
}

void TestConfigFunc() {
  printf("TestConfigFunc()\n");

  Context ctx;
  Json::Reader reader;
  Json::Value val;
  Res err;
  LispValue result;
  LispContext lc(&ctx, NULL);

  // Add some configs.
  Config* some_config = new Config();
  Config* some_other_config = new Config();
  ctx.AddConfig("some_config", some_config);
  ctx.AddConfig("some_other_config", some_other_config);

  // Bad arg count.
  assert(reader.parse("[\"config\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(!err.Ok());
  assert(result.type() == LispValue::NIL);

  // Test failed lookup.
  assert(reader.parse("[\"config\", \"some_name\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.type() == LispValue::NIL);

  // Test successful lookup.
  assert(reader.parse("[\"config\", \"some_config\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.type() == LispValue::CONFIG);
  assert(result.config_val() == some_config);
}

void TestConfigDetectedFunc() {
  printf("TestConfigDetecedFunc()\n");

  Context ctx;
  Json::Reader reader;
  Json::Value val;
  Res err;
  LispValue result;
  LispContext lc(&ctx, NULL);

  // Add some configs.
  Config* some_config = new Config();
  Config* some_other_config = new Config();
  ctx.AddConfig("some_config", some_config);
  ctx.AddConfig("some_other_config", some_other_config);

  some_config->SetVar("detected", "");
  some_other_config->SetVar("detected", "");

  // No args: return NIL.
  assert(reader.parse("[\"config_detected\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.type() == LispValue::NIL);

  // Return NIL, neither config is detected.
  assert(reader.parse("[\"config_detected\", \"some_config\", "
                      "\"some_other_config\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.type() == LispValue::NIL);

  // Return second arg, because it is detected.
  some_other_config->SetVar("detected", "1");
  assert(reader.parse("[\"config_detected\", \"some_config\", "
                      "\"some_other_config\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.type() == LispValue::CONFIG);
  assert(result.config_val() == some_other_config);
}

void TestFlagFunc() {
  printf("TestFlagFunc()\n");

  Context ctx;
  Json::Reader reader;
  Json::Value val;
  Res err;
  LispValue result;
  LispContext lc(&ctx, NULL);

  // Add some args to the context.
  const char* argv[] = {
    "dumbuild.exe",
    "--testflag=something",
    "--mode=other",
    "--emptyflag",
  };
  err = ctx.ProcessArgs(ARRAY_SIZE(argv), argv);

  // No args: fail.
  assert(reader.parse("[\"flag\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(!err.Ok());
  assert(result.type() == LispValue::NIL);

  // Return value of a flag that exists.
  assert(reader.parse("[\"flag\", \"mode\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.string_val() == "other");

  // Return NIL for a flag that does not exist.
  assert(reader.parse("[\"flag\", \"modexxx\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.type() == LispValue::NIL);

  // Return empty string for a flag that exists with no value.
  assert(reader.parse("[\"flag\", \"emptyflag\"]", val));
  err = Eval(&lc, LispValue(val), &result);
  assert(err.Ok());
  assert(result.string_val() == "");
}

void TestFillFunc() {
  printf("TestFillFunc()\n");

  Context ctx;
  Config cfg;
  Res err;
  Json::Reader reader;
  Json::Value val;
  LispContext lc(&ctx, &cfg);
  string result;

  // Simple pass-through, no substitution.
  assert(reader.parse("[\"fill\", \"bobo\"]", val));
  err = EvalToString(&lc, val, &result);
  assert(err.Ok());
  assert(result == "bobo");

  // Simple substitution.
  cfg.SetVar("basedir", "some_dir");
  assert(reader.parse("[\"fill\", \"${basedir}/something/something\"]", val));
  err = EvalToString(&lc, val, &result);
  assert(err.Ok());
  assert(result == "some_dir/something/something");

  // Failure due to undefined variable in template.
  assert(reader.parse("[\"fill\", \"blah blah ${undefined_var}\"]", val));
  err = EvalToString(&lc, val, &result);
  assert(!err.Ok());

  // Don't subtitute if varname contains invalid chars.
  assert(reader.parse("[\"fill\", \"blah blah ${*undefined_var}\"]", val));
  err = EvalToString(&lc, val, &result);
  assert(err.Ok());
  assert(result == "blah blah ${*undefined_var}");
}

void TestEval() {
  printf("TestEval()\n");
  TestLispValue();
  TestEvalToString();
  TestConfigFunc();
  TestConfigDetectedFunc();
  TestFlagFunc();
  TestFillFunc();
}
