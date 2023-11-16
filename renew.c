#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>

void clearDirectory(const char *path);
void copyFile(const char *sourcePath, const char *destinationPath);

int main()
{
    printf("Type of Renew\ndemo, 2009, exit\n\n");
    char option[10];
    scanf("%s", option);
    while(strcmp(option, "demo") * strcmp(option, "2009") != 0)
    {
        printf("Type of Renew\ndemo, 2009, exit\n\n");
        if (strcmp(option, "exit") == 0)
        {
            return 0;
        }
        scanf("%s", option);
    }
    remove("transactions.csv");
    clearDirectory("./message1");
    clearDirectory("./message2");
    copyFile("./backup/agent_message1.csv", "./message1/agent_message1.csv");
    copyFile("./backup/agent_message2.csv", "./message2/agent_message2.csv");
    copyFile("./backup/block_message1.csv", "./message1/block_message1.csv");
    copyFile("./backup/block_message2.csv", "./message2/block_message2.csv");
    copyFile("./backup/transactions_generate.csv", "transactions_generate.csv");

    if (strcmp(option, "demo") == 0)
    {
        copyFile("./backup/transactions.csv", "transactions.csv");
    }
    else
    {
        copyFile("./backup/transactions_2009.csv", "transactions.csv");
    }

    return 0;
}

// 清空指定目录
void clearDirectory(const char *path)
{
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (dir == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // 避免删除当前目录和上一级目录
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char filePath[PATH_MAX];
            snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

            if (unlink(filePath) != 0)
            {
                perror("Error deleting file");
                closedir(dir);
                exit(EXIT_FAILURE);
            }
        }
    }

    closedir(dir);
}

// 拷贝指定文件
void copyFile(const char *sourcePath, const char *destinationPath)
{
    FILE *sourceFile = fopen(sourcePath, "rb");

    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    FILE *destinationFile = fopen(destinationPath, "wb");

    if (destinationFile == NULL)
    {
        perror("Error opening destination file");
        fclose(sourceFile);
        exit(EXIT_FAILURE);
    }

    int ch;

    while ((ch = fgetc(sourceFile)) != EOF)
    {
        fputc(ch, destinationFile);
    }

    fclose(sourceFile);
    fclose(destinationFile);
}

