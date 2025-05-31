#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sqlite3.h>

#define PORT 8080
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 64  // SHA-256哈希长度为64字符
#define MAX_OPERATION_LEN 10
#define BUFFER_SIZE 1024

// 接收以null结尾的字符串
int receive_string(int client_socket, char *buffer, size_t max_len) {
    int total = 0;
    while (total < max_len-1) {
        int n = recv(client_socket, buffer + total, 1, 0);
        if (n <= 0) return -1;  // 接收错误或连接关闭
        if (buffer[total] == '\0') break;  // 遇到字符串结束符
        total++;
    }
    buffer[total] = '\0';  // 确保字符串正确终止
    return total;
}

void handle_client(int client_socket, sqlite3 *db) {
    char operation[MAX_OPERATION_LEN+1] = {0}; 
    char username[MAX_USERNAME_LEN+1] = {0};  
    char hashed_password[MAX_PASSWORD_LEN+1] = {0};  
    char response_message[BUFFER_SIZE] = {0};

    // 接收操作类型
    if (receive_string(client_socket, operation, sizeof(operation)) < 0) {
        strncpy(response_message, "接收操作类型失败", sizeof(response_message));
        goto send_response;
    }
    
    // 接收用户名
    if (receive_string(client_socket, username, sizeof(username)) < 0) {
        strncpy(response_message, "接收用户名失败", sizeof(response_message));
        goto send_response;
    }
    
    // 接收密码哈希
    if (receive_string(client_socket, hashed_password, sizeof(hashed_password)) < 0) {
        strncpy(response_message, "接收密码失败", sizeof(response_message));
        goto send_response;
    }

    printf("服务器接收到的操作: %s\n", operation);
    printf("服务器接收到的用户名: %s\n", username);
    
    if (strcmp(operation, "REGISTER") == 0) {
        // 使用参数化查询防止SQL注入
        char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
        sqlite3_stmt *stmt;
        
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            snprintf(response_message, sizeof(response_message), "数据库错误: %s", sqlite3_errmsg(db));
            goto cleanup;
        }
        
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hashed_password, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            snprintf(response_message, sizeof(response_message), "注册失败: %s", sqlite3_errmsg(db));
        } else {
            snprintf(response_message, sizeof(response_message), "注册成功");
        }
        
        sqlite3_finalize(stmt);
    } 
    else if (strcmp(operation, "LOGIN") == 0) {
        char *sql = "SELECT password FROM users WHERE username = ?;";
        sqlite3_stmt *stmt;
        
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            snprintf(response_message, sizeof(response_message), "数据库错误: %s", sqlite3_errmsg(db));
            goto cleanup;
        }
        
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *db_password = (const char *)sqlite3_column_text(stmt, 0);
            if (db_password && strcmp(db_password, hashed_password) == 0) {
                snprintf(response_message, sizeof(response_message), "登录成功");
            } else {
                snprintf(response_message, sizeof(response_message), "登录失败: 密码错误");
            }
        } else {
            snprintf(response_message, sizeof(response_message), "登录失败: 用户名不存在");
        }
        
        sqlite3_finalize(stmt);
    } 
    else {
        snprintf(response_message, sizeof(response_message), "未知操作类型");
    }

cleanup:
    printf("响应: %s\n", response_message);

send_response:
    // 发送响应，包含null终止符
    send(client_socket, response_message, strlen(response_message)+1, 0);
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_length = sizeof(client_address);

    // 初始化数据库
    sqlite3 *db;
    if (sqlite3_open("users.db", &db) != SQLITE_OK) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // 创建用户表
    char *create_table_sql = "CREATE TABLE IF NOT EXISTS users ("
                            "username TEXT PRIMARY KEY, "
                            "password TEXT NOT NULL);";
    
    if (sqlite3_exec(db, create_table_sql, NULL, NULL, NULL) != SQLITE_OK) {
        fprintf(stderr, "创建表失败: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // 创建服务器socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket 创建失败");
        sqlite3_close(db);
        return 1;
    }

    // 设置服务器地址
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // 绑定socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("绑定失败");
        close(server_socket);
        sqlite3_close(db);
        return 1;
    }

    // 监听连接
    if (listen(server_socket, 5) < 0) {
        perror("监听失败");
        close(server_socket);
        sqlite3_close(db);
        return 1;
    }

    printf("服务器启动，监听端口 %d...\n", PORT);

    // 主循环处理客户端连接
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_length);
        if (client_socket < 0) {
            perror("接受连接失败");
            continue;
        }

        printf("接收到来自 %s 的新连接\n", inet_ntoa(client_address.sin_addr));
        
        // 处理客户端请求
        handle_client(client_socket, db);
    }

    // 清理资源
    sqlite3_close(db);
    close(server_socket);
    return 0;
}
