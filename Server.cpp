#include <queue>
#include <io.h>
#include <conio.h>
#include "./FCT_Node/headers.h"
#include <map>
#include <string>

using namespace FCT;
using namespace std;

int main() {
    NodeCommon::Init();
    NodeEnvironment env;
    env.addModulePath("./node_modules");
    wcout.imbue(locale(".UTF-8"));

    env.code(R"(
const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const cors = require('cors');

const app = express();
app.use(cors());
app.use(express.json());

const server = http.createServer(app);

const io = socketIo(server, {
    cors: {
        origin: "*",
        methods: ["GET", "POST"]
    }
});

const users = {};

const systemLogs = [];

io.on('connection', (socket) => {
    console.log('新用户连接:', socket.id);

    socket.on('join', (username) => {
        users[socket.id] = username;
        console.log(`${username} 加入了聊天`);
        
        const message = `${username} 加入了聊天`;
        systemLogs.push({
            type: 'system',
            content: message,
            timestamp: new Date().toISOString()
        });

        io.emit('message', {
            type: 'system',
            content: message,
            timestamp: new Date().toISOString()
        });

        io.emit('userList', Object.values(users));
    });

    socket.on('message', (message) => {
        const username = users[socket.id];
        if (username) {
            console.log(`收到来自 ${username} 的消息:`, message);

            io.emit('message', {
                type: 'user',
                sender: username,
                content: message,
                timestamp: new Date().toISOString()
            });
        }
    });

    socket.on('privateMessage', (data) => {
        const sender = users[socket.id];
        const { recipient, message } = data;

        let recipientId = null;
        for (const [id, name] of Object.entries(users)) {
            if (name === recipient) {
                recipientId = id;
                break;
            }
        }
        
        if (recipientId) {
            io.to(recipientId).emit('privateMessage', {
                sender,
                content: message,
                timestamp: new Date().toISOString()
            });

            socket.emit('privateMessageSent', {
                recipient,
                content: message,
                timestamp: new Date().toISOString()
            });
        } else {
            socket.emit('error', {
                message: `用户 ${recipient} 不在线`
            });
        }
    });

    socket.on('disconnect', () => {
        const username = users[socket.id];
        if (username) {
            console.log(`${username} 离开了聊天`);
            
            const message = `${username} 离开了聊天`;
            systemLogs.push({
                type: 'system',
                content: message,
                timestamp: new Date().toISOString()
            });

            io.emit('message', {
                type: 'system',
                content: message,
                timestamp: new Date().toISOString()
            });

            delete users[socket.id];

            io.emit('userList', Object.values(users));
        }
    });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
    console.log(`聊天服务器运行在端口 ${PORT}`);
});

function getOnlineUsers() {
    return Object.values(users);
}

function getSystemLogs() {
    return systemLogs;
}

function broadcastMessage(message) {
    const logEntry = {
        type: 'system',
        content: message,
        timestamp: new Date().toISOString()
    };
    
    systemLogs.push(logEntry);
    io.emit('message', logEntry);
    return true;
}

function kickUser(username) {
    let kicked = false;
    let kickedId = null;
    
    for (const [id, name] of Object.entries(users)) {
        if (name === username) {
            kickedId = id;
            kicked = true;
            break;
        }
    }
    
    if (kicked && kickedId) {
        io.to(kickedId).emit('kicked');
        delete users[kickedId];
        
        const message = `${username} 已被踢出聊天室`;
        systemLogs.push({
            type: 'system',
            content: message,
            timestamp: new Date().toISOString()
        });
        
        io.emit('userList', Object.values(users));
        io.emit('message', {
            type: 'system',
            content: message,
            timestamp: new Date().toISOString()
        });
    }
    
    return kicked;
}
)");

    env.setup();
    
    wcout << L"聊天服务器正在启动..." << endl;

    std::string command;
    while (true) {
        env.tick();
        
        if (_kbhit()) {
            cin >> command;
            
            if (command == "exit") {
                wcout << L"关闭服务器..." << endl;
                break;
            } 
            else if (command == "users") {
                JSArray users = env.callFunction<JSArray>("getOnlineUsers");
                wcout << L"当前在线用户:" << endl;
                for (int i = 0; i < users.length(); i++) {
                    cout << (i + 1) << ". " << users.get<std::string>(i) << endl;
                }
            } 
            else if (command == "logs") {
                JSArray logs = env.callFunction<JSArray>("getSystemLogs");
                wcout << L"系统日志:" << endl;
                for (int i = 0; i < logs.length(); i++) {
                    JSObject log = logs.get<JSObject>(i);
                    string content = log.get<string>("content");
                    string timestamp = log.get<string>("timestamp");
                    cout << "[" << timestamp << "] " << content << endl;
                }
            }
            else if (command == "broadcast") {
                string message;
                cin >> message;
                
                bool result = env.callFunction<bool>("broadcastMessage", message);
                if (result) {
                    wcout << L"消息已广播" << endl;
                } else {
                    wcout << L"广播失败" << endl;
                }
            } 
            else if (command == "kick") {
                string username;
                wcout << L"输入要踢出的用户名: ";
                cin >> username;
                
                bool result = env.callFunction<bool>("kickUser", username);
                if (result) {
                    wcout << L"用户 " << username.c_str() << L" 已被踢出" << endl;
                } else {
                    wcout << L"找不到用户 " << username.c_str() << endl;
                }
            } 
            else if (command == "help") {
                wcout << L"可用命令:" << endl;
                wcout << L"users - 显示在线用户列表" << endl;
                wcout << L"logs - 显示系统日志" << endl;
                wcout << L"broadcast - 发送系统广播消息" << endl;
                wcout << L"kick - 踢出指定用户" << endl;
                wcout << L"help - 显示帮助信息" << endl;
                wcout << L"exit - 关闭服务器" << endl;
            } 
            else {
                wcout << L"未知命令，输入 'help' 获取帮助" << endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    env.stop();
    return 0;
}