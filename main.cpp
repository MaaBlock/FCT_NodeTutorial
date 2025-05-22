#include <iostream>
#include <node.h>
#include <v8.h>
#include <uv.h>
#include <string>
#include <thread>
#include <vector>
using namespace v8;
using namespace node;



int RunNodeInstance(MultiIsolatePlatform* platform,
                    const std::vector<std::string>& args,
                    const std::vector<std::string>& exec_args) {
    int exit_code = 0;

    for (auto &arg : args)
    {
        std::cout << "Args: " << arg << std::endl;
    }
    for (auto &arg : exec_args)
    {
        std::cout << "Exec Args: " << arg << std::endl;
    }

  std::vector<std::string> errors;
  std::unique_ptr<CommonEnvironmentSetup> setup =
      CommonEnvironmentSetup::Create(platform, &errors, args, exec_args);
  if (!setup) {
    for (const std::string& err : errors)
      fprintf(stderr, "%s: %s\n", args[0].c_str(), err.c_str());
    return 1;
  }

  Isolate* isolate = setup->isolate();
  Environment* env = setup->env();

  {
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    Context::Scope context_scope(setup->context());
    MaybeLocal<Value> loadenv_ret = node::LoadEnvironment(
        env,
        "const publicRequire ="
        "  require('node:module').createRequire(process.cwd() + '/');"
        "globalThis.require = publicRequire;"
        "require('node:vm').runInThisContext(process.argv[1]);");

    if (loadenv_ret.IsEmpty())
      return 1;

    exit_code = node::SpinEventLoop(env).FromMaybe(1);

    node::Stop(env);
  }

  return exit_code;
}
namespace FCT
{
    std::ostream& fout = std::cout;
}
namespace FCT
{
    int GetOptimalNodeThreadPoolSize() {
        unsigned int hardware_concurrency = std::thread::hardware_concurrency();

        if (hardware_concurrency == 0) {
            hardware_concurrency = 4;
        }

        return hardware_concurrency;
    }
    class NodeEnvironment;
    class NodeCommon
    {
    private:
        static std::shared_ptr<node::InitializationResult> g_initializationResultl;
        static std::unique_ptr<MultiIsolatePlatform> g_platform;
    public:
        static std::shared_ptr<node::InitializationResult> GetInitializationResult() {
            return g_initializationResultl;
        }
        static const std::unique_ptr<MultiIsolatePlatform>& GetPlatform()
        {
            return g_platform;
        }
        static void Init()
        {
            auto args = std::vector<std::string>();
            args.push_back("FCTApp");
            std::shared_ptr<node::InitializationResult> result =
                node::InitializeOncePerProcess(args, {
                  node::ProcessInitializationFlags::kNoInitializeV8,
                  node::ProcessInitializationFlags::kNoInitializeNodeV8Platform
                });
            for (const std::string& error : result->errors())
            {
                fout << error << std::endl;
            }
            if (result->early_return() != 0) {
                fout << "fetal error: Init FCT Node Intergration faild, error code:" << result->exit_code() << std::endl;
            }
            g_initializationResultl = result;
            std::unique_ptr<MultiIsolatePlatform> platform =
                MultiIsolatePlatform::Create(GetOptimalNodeThreadPoolSize());
            V8::InitializePlatform(platform.get());
            V8::Initialize();
            g_platform = std::move(platform);
        }
        static void term()
        {
            V8::Dispose();
            V8::DisposePlatform();

            node::TearDownOncePerProcess();
        }
        void init()
        {

        }

    };
    std::shared_ptr<node::InitializationResult> NodeCommon::g_initializationResultl = nullptr;
    std::unique_ptr<MultiIsolatePlatform> NodeCommon::g_platform;
}
namespace FCT
{
    class NodeEnvironment
    {
    protected:
        std::vector<std::string> m_args;
        std::vector<std::string> m_excuteArgs;
        std::vector<std::string> m_modulePaths;
    public:
        NodeEnvironment()
        {

        }
        void args(const std::vector<std::string> args)
        {
            m_args = args;
        }
        void excuteArgs(const std::vector<std::string> executeArgs)
        {
            m_excuteArgs = executeArgs;
        }
        void addModulePath(const std::string& path)
        {
            m_modulePaths.push_back(path);
        }
        int excute()
        {
            auto platform = NodeCommon::GetPlatform().get();
            auto args = m_args;
            auto exec_args = m_excuteArgs;
            int exitCode = 0;

            for (auto &arg : args)
            {
                std::cout << "Args: " << arg << std::endl;
            }
            for (auto &arg : exec_args)
            {
                std::cout << "Exec Args: " << arg << std::endl;
            }

            std::vector<std::string> errors;
            std::unique_ptr<CommonEnvironmentSetup> setup =
                CommonEnvironmentSetup::Create(platform, &errors, args, exec_args);
            if (!setup) {
                for (const std::string& err : errors)
                    fprintf(stderr, "%s: %s\n", args[0].c_str(), err.c_str());
                return 1;
            }

            Isolate* isolate = setup->isolate();
            Environment* env = setup->env();

            {
                Locker locker(isolate);
                Isolate::Scope isolate_scope(isolate);
                HandleScope handle_scope(isolate);
                Context::Scope context_scope(setup->context());

                std::string setupModulePaths = "";
                for (const auto& path : m_modulePaths) {
                    setupModulePaths += "module.paths.push('" + path + "');\n";
                }

                MaybeLocal<Value> loadenv_ret = node::LoadEnvironment(
                    env,
                    "const publicRequire ="
                    "  require('node:module').createRequire(process.cwd() + '/');"
                    "globalThis.require = publicRequire;"
                    + setupModulePaths +
                    "require('node:vm').runInThisContext(process.argv[1]);");

                if (loadenv_ret.IsEmpty())
                    return 1;

                exitCode = node::SpinEventLoop(env).FromMaybe(1);

                node::Stop(env);
            }

            return exitCode;
        }
    };
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
int main() {
    Init();
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