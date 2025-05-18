#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gmssl/sm3.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define SALT "random_salt"
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_HASH_LEN 65

// 发送字符串并确保以null结尾
void send_string(int sock, const char *str) {
    // 发送字符串内容包括null终止符
    send(sock, str, strlen(str)+1, 0);
}

void hash_password(const char *password, const char *salt, char *output) {
    SM3_CTX ctx;
    unsigned char digest[32];
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", password, salt);

    sm3_init(&ctx);
    sm3_update(&ctx, (uint8_t *)combined, strlen(combined));
    sm3_finish(&ctx, digest);

    for (int i = 0; i < 32; i++) {
        snprintf(output + (i * 2), 3, "%02x", digest[i]);
    }
    output[64] = '\0'; // 确保null终止
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char choice, username[MAX_USERNAME_LEN], password[MAX_PASSWORD_LEN];
    char hashed_password[MAX_HASH_LEN] = {0};
    char server_response[256] = {0};

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket 创建失败");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("无法连接到服务器");
        return 1;
    }

    printf("选择操作: [R] 注册 | [L] 登录: ");
    scanf(" %c", &choice);
    printf("用户名: ");
    scanf("%49s", username);  // 限制输入长度
    printf("密码: ");
    scanf("%49s", password);  // 限制输入长度

    hash_password(password, SALT, hashed_password);

    // 发送操作类型
    if (choice == 'R' || choice == 'r') {
        send_string(client_socket, "REGISTER");
    } else {
        send_string(client_socket, "LOGIN");
    }
    
    // 发送用户名和密码
    send_string(client_socket, username);
    send_string(client_socket, hashed_password);

    // 接收服务器响应
    if (recv(client_socket, server_response, sizeof(server_response)-1, 0) > 0) {
        printf("服务器回复: %s\n", server_response);
    } else {
        printf("未收到服务器响应\n");
    }

    close(client_socket);
    return 0;
}
