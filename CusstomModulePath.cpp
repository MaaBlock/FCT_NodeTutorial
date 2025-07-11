#include <queue>

#include <io.h>
#include "./FCT_Node/headers.h"
using namespace FCT;
using namespace std;
int main()
{
    NodeCommon::Init();
    NodeEnvironment env;
    env.addModulePath("F:/FCT_NodeTest/loadModules/node_modules");
    wcout.imbue(locale(".UTF-8"));
    env.code(R"(
const Module = require('module');
Module.registerHooks({
  resolve: (specifier, context, nextResolve) => {
    console.log('Resolving', specifier);
    return nextResolve(specifier, context);
  }
});
console.log('paths:', Module);
console.log('Global paths:', Module.globalPaths);
const uuid = require('uuid')
)");
    env.setup();

    env.stop();
    return 0;
}