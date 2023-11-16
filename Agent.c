#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char* lookup = "lookup";
char* transaction = "transaction";

void loopActivate();
void writeToCSV(char* filename, char* operation);

int main()
{
    loopActivate();
    return 0;
}

// 启动程序
void loopActivate()
{
    printf("input format\nnode_index operation\nnode_index: 1, 2\ntransaction:1-lookup, 2-transaction\nexit:0 0\n\n");
    int node_index;
    int sign;
    scanf("%d %d", &node_index, &sign);

    while (node_index && sign)
    {
        // 循环发送信息
        char filename[30];
        if (node_index == 1 || node_index == 2)
        {
            sprintf(filename, "./message%d/agent_message%d.csv", node_index, node_index);
            if (sign == 1)
            {
                writeToCSV(filename, lookup);
            }
            else if(sign == 2)
            {
                writeToCSV(filename, transaction);
            }
            else
            {
                printf("operation error\n");    
            }
        }
        else
        {
            printf("node error\n");    
        }

        // 下一步输入
        scanf("%d %d", &node_index, &sign);
    }
}

// 将可会消息存入csv
void writeToCSV(char* filename, char* operation)
{
    // 打开文件，如果文件不存在则创建，如果存在则追加写入
    FILE *file = fopen(filename, "a");

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // 写入filename并在末尾加上换行符
    fprintf(file, "%s\n", operation);

    // 关闭文件
    fclose(file);
}
