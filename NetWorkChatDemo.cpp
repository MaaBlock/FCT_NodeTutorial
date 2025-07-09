#include <queue>
#include <io.h>
#include "./FCT_Node/headers.h"
#include <conio.h>
#include <map>
#include <string>

using namespace FCT;
using namespace std;

class Input
{
public:
    Input()
    {

    }
    Input(const std::string& input) : input(input)
    {

    }
    ~Input()
    {

    }
    std::string input;
};


boost::lockfree::queue<Input*,boost::lockfree::capacity<100>> inputs;

std::string GetInput()
{
    Input* input;
    while (!inputs.pop(input))
    {

    }
    std::string result = input->input;
    delete input;
    return result;
}

int main() {
    bool running = true;
    NodeCommon::Init();
    NodeEnvironment env;
    env.addModulePath("./node_modules");
    wcout.imbue(locale(".UTF-8"));

    env.code(R"(
const socketIo = require('socket.io-client');

let socket;
let connected = false;
let username = '';

const messageQueue = [];
const userList = [];
let kicked = false;

function connect(serverUrl, user) {
    return new Promise((resolve, reject) => {
        try {
            username = user;
            socket = socketIo(serverUrl);

            socket.on('connect', () => {
                console.log('已连接到服务器');
                connected = true;

                socket.emit('join', username);
                resolve(true);
            });

            socket.on('connect_error', (error) => {
                console.error('连接错误:', error);
                connected = false;
                reject('连接错误: ' + error.message);
            });

            setupMessageHandlers();

        } catch (error) {
            console.error('连接失败:', error);
            reject('连接失败: ' + error.message);
        }
    });
}

function setupMessageHandlers() {
    socket.on('message', (data) => {
        let formattedMessage = '';

        if (data.type === 'system') {
            formattedMessage = `[系统] ${data.content}`;
        } else if (data.type === 'user') {
            formattedMessage = `[${data.sender}] ${data.content}`;
        }

        messageQueue.push({
            type: data.type,
            message: formattedMessage,
            timestamp: data.timestamp
        });
    });

    socket.on('privateMessage', (data) => {
        const formattedMessage = `[私聊][${data.sender}] ${data.content}`;
        messageQueue.push({
            type: 'private',
            message: formattedMessage,
            timestamp: data.timestamp
        });
    });

    socket.on('privateMessageSent', (data) => {
        const formattedMessage = `[私聊给 ${data.recipient}] ${data.content}`;
        messageQueue.push({
            type: 'privateSent',
            message: formattedMessage,
            timestamp: data.timestamp
        });
    });

    socket.on('userList', (users) => {
        userList.length = 0;
        users.forEach(user => userList.push(user));
    });

    socket.on('kicked', () => {
        messageQueue.push({
            type: 'kicked',
            message: '您已被管理员踢出聊天室',
            timestamp: new Date().toISOString()
        });
        kicked = true;
        disconnect();
    });

    socket.on('error', (data) => {
        messageQueue.push({
            type: 'error',
            message: data.message,
            timestamp: new Date().toISOString()
        });
    });
}

function hasMessages() {
    return messageQueue.length > 0;
}

function getNextMessage() {
    return messageQueue.shift();
}

function getUserList() {
    return userList;
}

function wasKicked() {
    return kicked;
}

function sendMessage(message) {
    if (!connected || !socket) {
        return false;
    }

    try {
        socket.emit('message', message);
        return true;
    } catch (error) {
        console.error('发送消息失败:', error);
        return false;
    }
}

function sendPrivateMessage(recipient, message) {
    if (!connected || !socket) {
        return false;
    }

    try {
        socket.emit('privateMessage', {
            recipient,
            message
        });
        return true;
    } catch (error) {
        console.error('发送私聊消息失败:', error);
        return false;
    }
}

function disconnect() {
    if (socket) {
        socket.disconnect();
        connected = false;
        return true;
    }
    return false;
}

function isConnected() {
    return connected;
}

function getUsername() {
    return username;
}
)");

    env.setup();

    wcout << L"聊天客户端已启动" << endl;
    wcout << L"请输入服务器地址 (例如: http://localhost:3000): ";
    string serverUrl;
    cin >> serverUrl;//输入服务器地址

    wcout << L"请输入您的用户名: ";
    string username;
    cin >> username;


    std::thread read([&]()
    {
        std::string input;
        while (running)
        {
            cin >> input;
            inputs.push(new Input(input));
        }
    });

    wcout << L"正在连接到服务器..." << endl;
    auto promise = env.callFunction<JSPromise>("connect", serverUrl, username);
//
    while (!promise.isSettled()) {
        env.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    if (promise.isFulfilled()) {
        wcout << L"已成功连接到服务器!" << endl;

        bool running = true;
        while (running) {
            env.tick();

            if (env.callFunction<bool>("wasKicked")) {
                wcout << L"您已被管理员踢出聊天室" << endl;
                break;
            }
            while (env.callFunction<bool>("hasMessages"))
            {
                JSObject message = env.callFunction<JSObject>("getNextMessage");
                string messageText = message["message"];
                cout << messageText << endl;
            }
            while (!inputs.empty())
            {
                string command = GetInput();//获取输入
                if (command == "exit") {
                    wcout << L"断开连接并退出..." << endl;
                    env.callFunction<bool>("disconnect");
                    running = false;
                }
                else if (command == "users") {
                    JSArray users = env.callFunction<JSArray>("getUserList");
                    wcout << L"当前在线用户:" << endl;
                    for (int i = 0; i < users.length(); i++) {
                        cout << (i + 1) << ". " << users.get<std::string>(i) << endl;
                    }
                }
                else if (command == "msg") {
                    string message = GetInput();
                    bool result = env.callFunction<bool>("sendMessage", message);
                    if (!result) {
                        wcout << L"发送消息失败" << endl;
                    }
                }
                else if (command == "pm") {
                    string recipient = GetInput();
                    string message = GetInput();
                    bool result = env.callFunction<bool>("sendPrivateMessage", recipient, message);
                    if (!result) {
                        wcout << L"发送私聊消息失败" << endl;
                    }
                }
                else if (command == "help") {
                    wcout << L"可用命令:" << endl;
                    wcout << L"users - 显示在线用户列表" << endl;
                    wcout << L"msg - 发送公共消息" << endl;
                    wcout << L"pm - 发送私聊消息" << endl;
                    wcout << L"help - 显示帮助信息" << endl;
                    wcout << L"exit - 断开连接并退出" << endl;
                }
                else {
                    wcout << L"未知命令，输入 'help' 获取帮助" << endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    } else {
        wcout << L"连接失败: " << promise.getError().c_str() << endl;
    }

    env.stop();
    return 0;
}