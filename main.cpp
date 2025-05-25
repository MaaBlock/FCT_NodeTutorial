#include "./FCT_Node/headers.h"
using namespace FCT;
using namespace std;
int main()
{
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

    console.log('Loaded modules:', Object.keys(require.cache));
} catch (err) {
    console.error('Error loading npm modules:', err.message);
}
)"
    };
    env.args(args);
    env.excuteArgs(std::vector<std::string>({}));
    env.setup();
    //env.executeArg();
    /*
    env.execute(R"(
    const lodash = require('lodash');
    console.log('test');
)");*/
    env.execute(R"(
    const lodash = require('lodash');
    const chalk = require('chalk');
    function isValidVersion(version) {
        const versionPattern = /^\d+\.\d+\.\d+$/;
        const isValid = versionPattern.test(version);
        console.log(chalk.yellow(`版本号 ${version} 验证结果: ${isValid ? '有效' : '无效'}`));
        return isValid;
    }
    function foo(){
        console.log(chalk.blue.bold('hello from nodejs'));
    }
    try {
    console.log('Lodash loaded successfully');
    console.log('Random number from lodash:', lodash.random(1, 100));

    console.log(chalk.green('This text is green!'));
    console.log(chalk.blue.bold('This text is blue and bold!'));

    console.log('Loaded modules:', Object.keys(require.cache));
} catch (err) {
    console.error('Error loading npm modules:', err.message);
}
)");
    env.callFunction("foo");
    if (env.callFunction<bool>("isValidVersion","0.1.0"))
    {
        cout << "Version is valid" << endl;
    } else
    {
        cout << "Version is not valid" << endl;
    }
    Sleep(1000);
    env.stop();
    return 0;
}