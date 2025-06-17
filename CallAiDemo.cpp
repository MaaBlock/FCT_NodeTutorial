#include <queue>

#include <io.h>
#include "./FCT_Node/headers.h"
using namespace FCT;
using namespace std;
int main()
{
    NodeCommon::Init();
    NodeEnvironment env;
    env.addModulePath("./node_modules");
    wcout.imbue(locale(".UTF-8"));
    env.code(R"(
const OpenAI = require("openai");
const axios = require('axios');
const cheerio = require('cheerio');

const openai = new OpenAI({
        baseURL: 'https://api.deepseek.com',
        apiKey: ''
});


async function askAi(system, question) {
    return new Promise(async (resolve, reject) => {
        try {
            const completion = await openai.chat.completions.create({
                messages: [
                  { role: "system", content: system },
                  { role: "user", content: question }
                ],
                model: "deepseek-chat",
            });

            resolve(completion.choices[0].message.content);
        } catch (error) {
            console.error("Error asking AI:", error);
            reject("Error: " + error.message);
        }
    });
}

async function searchBaidu(keyword) {
    return new Promise(async (resolve, reject) => {
        try {
            const searchUrl = `https://www.baidu.com/s?wd=${encodeURIComponent(keyword)}`;

            const headers = {
                'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
                'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
                'Accept-Language': 'zh-CN,zh;q=0.9,en;q=0.8'
            };

            const response = await axios.get(searchUrl, { headers });

            const $ = cheerio.load(response.data);

            const titles = [];
            $('.result h3 a, .c-container .t a').each((index, element) => {
                if (index < 10) {
                    titles.push($(element).text().trim());
                }
            });

            resolve(titles);
        } catch (error) {
            console.error("Error searching Baidu:", error);
            reject("Error: " + error.message);
        }
    });
}

)");
    env.setup();

/*
 *  searchBaidu(字符串) 百度搜索字符串 返回Promise 结果是 前10个百度标题 标题数组(JSArray)
 *  askAi(字符串system, 字符串问题) 问deepseek，返回 Promise 结果是 ai回答（字符串）
 */

    std::string command;
    std::string system = "你是一个助手";
    while (true)
    {
        cin >> command;
        if (command == "exit")
        {
            wcout << L"退出" << endl;
            break;
        } else if (command == "ask")
        {
            string question;
            cin >> question;
            auto promise = env.callFunction<JSPromise>("askAi",
              system,
              question
              );
            wcout << L"AI正在回答中，过程可能要20～25s" << endl;
            while (!promise.isSettled())
            {
                env.tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            if (promise.isFulfilled())
            {
                wcout << L"AI回答： "  << endl;
                cout << promise.getResult<std::string>() << endl;
            } else
            {
                wcout << L"发生错误" << endl;
                cout << promise.getError() << endl;
            }
        }
        else if (command == "system")
        {
            string newSystem;
            if (cin.peek() == '\n' || cin.eof()) {
                wcout << L"当前系统提示词: " << endl;
                cout << system << endl;
            } else {
                getline(cin, newSystem);
                system = newSystem;
                wcout << L"系统提示词已更新为: " << endl;
                cout << system << endl;
            }
        }
        else if (command == "baidu")
        {
            string keyword;
            cin >> keyword;

            wcout << L"正在搜索百度，请稍候..." << endl;
            auto promise = env.callFunction<JSPromise>("searchBaidu", keyword);

            while (!promise.isSettled())
            {
                env.tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            if (promise.isFulfilled())
            {
                wcout << L"百度搜索结果前10条标题：" << endl;
                JSArray results = promise.getResult<JSArray>();
                for (int i = 0; i < results.length(); i++)
                {
                    cout << (i + 1) << ". " << results.get<std::string>(i) << endl;
                }
            }
            else
            {
                wcout << L"搜索失败" << endl;
                cout << promise.getError() << endl;
            }
        }
        else if (command == "help")
        {
            wcout << L"ask [问题]: 向AI提问" << endl;
            wcout << L"help: 帮助" << endl;
            wcout << L"exit: 退出" << endl;
        }
        else
        {
            wcout << L"未知命令" << endl;
        }
        env.tick();
    }


    env.stop();
    return 0;
}