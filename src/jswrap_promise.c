/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2016 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * ES6 Promise implementation
 * ----------------------------------------------------------------------------
 */
#include "jswrap_promise.h"
#include "jsparse.h"
#include "jsinteractive.h"
#include "jswrapper.h"

#define JS_PROMISE_THEN_NAME JS_HIDDEN_CHAR_STR"then"
#define JS_PROMISE_CATCH_NAME JS_HIDDEN_CHAR_STR"catch"

/*JSON{
  "type" : "class",
  "class" : "Promise"
}
This is the built-in class for ES6 Promises
*/

void jswrap_promise_resolve(JsVar *promise, JsVar *data) {
  JsVar *fn = jsvObjectGetChild(promise, JS_PROMISE_THEN_NAME, 0);
  jsvUnLock(jspeFunctionCall(fn, 0, promise, false, 1, &data));
  jsvUnLock(fn);
}
void jswrap_promise_queueresolve(JsVar *promise, JsVar *data) {
  JsVar *fn = jsvNewNativeFunction((void (*)(void))jswrap_promise_resolve, JSWAT_VOID|JSWAT_THIS_ARG|(JSWAT_JSVAR<<JSWAT_BITS));
  if (!fn) return;
  jsvObjectSetChild(fn, JSPARSE_FUNCTION_THIS_NAME, promise);
  jsiQueueEvents(promise, fn, &data, 1);
  jsvUnLock(fn);
}

void jswrap_promise_reject(JsVar *promise, JsVar *data) {
  JsVar *fn = jsvObjectGetChild(promise, JS_PROMISE_CATCH_NAME, 0);
  jsvUnLock(jspeFunctionCall(fn, 0, promise, false, 1, &data));
  jsvUnLock(fn);
}
void jswrap_promise_queuereject(JsVar *promise, JsVar *data) {
  JsVar *fn = jsvNewNativeFunction((void (*)(void))jswrap_promise_reject, JSWAT_VOID|JSWAT_THIS_ARG|(JSWAT_JSVAR<<JSWAT_BITS));
  if (!fn) return;
  jsvObjectSetChild(fn, JSPARSE_FUNCTION_THIS_NAME, promise);
  jsiQueueEvents(promise, fn, &data, 1);
  jsvUnLock(fn);
}

/*JSON{
  "type" : "constructor",
  "class" : "Promise",
  "name" : "Promise",
  "generate" : "jswrap_promise_constructor",
  "params" : [
    ["executor","JsVar","A function of the form `function (resolve, reject)`"]
  ],
  "return" : ["JsVar","A Promise"]
}
Create a new Promise. The executor function is executed immediately (before the constructor even returns)
and
 */
JsVar *jswrap_promise_constructor(JsVar *executor) {
  JsVar *obj = jspNewObject(0, "Promise");
  if (obj) {
    // create resolve and reject
    JsVar *args[2] = {
        jsvNewNativeFunction((void (*)(void))jswrap_promise_queueresolve, JSWAT_VOID|JSWAT_THIS_ARG|(JSWAT_JSVAR<<JSWAT_BITS)),
        jsvNewNativeFunction((void (*)(void))jswrap_promise_queuereject, JSWAT_VOID|JSWAT_THIS_ARG|(JSWAT_JSVAR<<JSWAT_BITS))
    };
    // bind 'this' to functions
    if (args[0]) jsvObjectSetChild(args[0], JSPARSE_FUNCTION_THIS_NAME, obj);
    if (args[1]) jsvObjectSetChild(args[1], JSPARSE_FUNCTION_THIS_NAME, obj);
    // call the executor
    jsvUnLock(jspeFunctionCall(executor, 0, obj, false, 2, args));
    jsvUnLockMany(2, args);
  }
  return obj;
}

/*JSON{
  "type" : "staticmethod",
  "class" : "Promise",
  "name" : "all",
  "generate" : "jswrap_promise_all",
  "params" : [
    ["promises","JsVar","An array of promises"]
  ],
  "return" : ["JsVar","A new Promise"]
}
Return a new promise that is resolved when all promises in the supplied
array are resolved.
*/
JsVar *jswrap_promise_all(JsVar *arr) {
  jsError("Unimplemented\n");
  return 0;
}

/*JSON{
  "type" : "method",
  "class" : "Promise",
  "name" : "then",
  "generate" : "jswrap_promise_then",
  "params" : [
    ["callback","JsVar","A callback that is called when this promise is resolved"]
  ]
}
 */
void jswrap_promise_then(JsVar *parent, JsVar *callback) {
  if (!jsvIsFunction(callback)) {
    jsExceptionHere(JSET_TYPEERROR, "Callback must be a function, got %t", callback);
    return;
  }
  jsvObjectSetChild(parent, JS_PROMISE_THEN_NAME, callback);
}

/*JSON{
  "type" : "method",
  "class" : "Promise",
  "name" : "catch",
  "generate" : "jswrap_promise_catch",
  "params" : [
    ["callback","JsVar","A callback that is called when this promise is rejected"]
  ]
}
 */
void jswrap_promise_catch(JsVar *parent, JsVar *callback) {
  if (!jsvIsFunction(callback)) {
    jsExceptionHere(JSET_TYPEERROR, "Callback must be a function, got %t", callback);
    return;
  }
  jsvObjectSetChild(parent, JS_PROMISE_CATCH_NAME, callback);
}
