#include "./FCT_Node/headers.h"
using namespace FCT;
using namespace std;
int main()
{
    NodeCommon::Init();
    cout << "please install must libary by using npm" << endl;
    NodeEnvironment env;
    env.addModulePath("./node_modules");
    env.code(R"(

    const lodash = require('lodash');
    const chalk = require('chalk');
    function isValidVersion(version) {
        const versionPattern = /^\d+\.\d+\.\d+$/;
        const isValid = versionPattern.test(version);
        console.log(chalk.yellow(`Version ${version} isValid: ${isValid ? 'Valid' : 'Invalid'}`));
        return isValid;
    }

    function foo(){
        console.log(chalk.blue.bold('hello from nodejs'));
    }
    function getProjectInfo(name, version) {
        return {
            name: name || 'FCT Project',
            version: version || '1.0.0',
            description: 'A Node.js integration project',
            dependencies: {
                lodash: '^4.17.21',
                chalk: '^4.1.2'
            },
            features: ['JavaScript Integration', 'NPM Support', 'V8 Engine'],
            isStable: true,
            stats: {
                codeLines: 1500,
                testCoverage: 85.5
            }
        };
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
    env.setup();
    env.callFunction("foo");
    if (env.callFunction<bool>("isValidVersion","0.1.0"))
    {
        cout << "Version is valid" << endl;
    } else
    {
        cout << "Version is not valid" << endl;
    }
    {
        JSObject projectInfo = env.callFunction<JSObject>("getProjectInfo", "FCT-Test", "2.0.0");
        std::string name = projectInfo["name"];
        JSObject stats = projectInfo["stats"];
        for (auto &name : stats.getPropertyNames())
        {
            cout << "stats property:" << name << endl;
        }
        cout << "Name: " << name << endl;
    }
    env.stop();
    return 0;
}