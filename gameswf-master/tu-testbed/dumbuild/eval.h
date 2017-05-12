// eval.h  -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Functions for evaluating Lisp-like Json expressions.

#ifndef EVAL_H_
#define EVAL_H_

#include <json/json.h>
#include "dmb_types.h"
#include "res.h"

// Must be called at program startup.
void InitEval();

class Config;
class Context;

// Evaluate the given Json value to a string.  Returns an error if the
// result is not a string.
Res EvalToString(const Context* ctx, Config* cfg, const Json::Value& val,
		 string* out);

#endif  // EVAL_H_
