#include "./FCT_Node/headers.h"
using namespace v8;
using namespace node;
namespace FCT
{
}
namespace FCT
{
    class NodeCallBack
    {
    protected:

    public:

    };
}
void Init()
{
    FCT::NodeCommon::Init();
}
using namespace FCT;
using namespace std;
int main() {
    cout << "please install must libary by using npm" << endl;
    NodeEnvironment env;
    env.addModulePath("./node_modules");
    std::vector<std::string> args = {
        "project",
        R"(
try {
    const _ = require('lodash');
    console.log('Lodash loaded successfully');
    console.log('Random number from lodash:', _.random(1, 100));

    const chalk = require('chalk');
    console.log(chalk.green('This text is green!'));
    console.log(chalk.blue.bold('This text is blue and bold!'));

    console.log('Module search paths:');
    console.log(module.paths);
} catch (err) {
    console.error('Error loading npm modules:', err.message);
}
)"
    };
    env.args(args);
    env.excuteArgs(std::vector<std::string>({}));
    int ret = env.excute();
    return ret;
}