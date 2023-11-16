#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"cJSON.h"

// 定义执行次数Height
int Height = -1;

// 定义一个交易
typedef struct Transaction
{
    char* txid;  // 交易的编号，具有唯一性
    int input_count;  // inputs的数量，本实验可以忽略
    int output_count;  // outpus的数量，本实验可以忽略
    int is_coinbase;  // 表示是否为coinbase交易(1为coinbase交易，0为非coinbase交易)
} Transaction;

// 先将需要的Transaction组织称一个链表，方便存储和查改
typedef struct Node_Transaction
{
    int index;
    Transaction transaction;
    struct Node_Transaction* next;
} Node_Transaction;

// 定义一个区块
typedef struct Block
{
    int height;  // 当前块的高度，一条链上每个区块的Height均不相同
    unsigned long hash;  // 本区块的哈希值
    unsigned long prevHash;  // 前一个区块的哈希值
    int transaction_count;  // 该区块的交易数量
    struct Transaction transactions[5];  // 该区块的所有交易，最多不超过5个
    struct Block* prev;
    struct Block* next;
} Block;

// csv文件读写函数
void deleteLines(const char *filename, int n);
void writeFilenameToCSV(const char *filename);
char* getMessage(const char* filename);

// json文件读写函数
cJSON* transactionToJSON(Transaction* transaction);
cJSON* blockToJSON(Block* block);
void writeJsonToFile(cJSON *jsonObject, const char *filename);
void parseTransactionFromJSON(cJSON* jsonTransaction, struct Transaction* transaction);
Block* readBlockFromJSONFile(const char* filename);

// 哈希值生成函数
unsigned long hash_djb2(unsigned int num);

// 区块链函数
Node_Transaction* select_Transaction(int number_select);
Node_Transaction* select_Transaction_generate(int number_select);
Block* New_Block(Block* list, Node_Transaction* select);
Block* Add_Block(Block* list);
Block* createBlocklist();
void lengthLinkedList(Block* list);
int countTransaction(Block* list);
void infoLinkedList(Block* list);

// 功能实现函数
void loopActivate(Block* list);


int main()
{
    Block* list = createBlocklist();
    loopActivate(list);
    return 0;
}

// 删除表头后的n行
void deleteLines(const char *filename, int n)
{
    n += 1; // 跳过表头

    // 打开原始文件
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // 创建临时文件
    FILE *tempFile = fopen("temp.csv", "w");
    if (tempFile == NULL) {
        perror("Error creating temporary file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // 保留第一行（表头）
    char line[256]; // 适当选择缓冲区大小
    if (fgets(line, sizeof(line), file) != NULL) {
        fputs(line, tempFile);
    }

    // 跳过后续的n-1行
    for (int i = 0; i < n - 1; ++i) {
        if (fgets(line, sizeof(line), file) == NULL) {
            break; // 文件可能比n行短
        }
    }

    // 将剩余内容复制到临时文件
    while (fgets(line, sizeof(line), file) != NULL) {
        fputs(line, tempFile);
    }

    // 关闭文件
    fclose(file);
    fclose(tempFile);

    // 删除原始文件
    remove(filename);

    // 将临时文件重命名为原始文件
    if (rename("temp.csv", filename) != 0) {
        perror("Error renaming file");
        exit(EXIT_FAILURE);
    }
}

// 将文件名存入csv
void writeFilenameToCSV(const char *filename)
{
    // 打开文件，如果文件不存在则创建，如果存在则追加写入
    FILE *file = fopen("./message2/block_message2.csv", "a");

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // 写入filename并在末尾加上换行符
    fprintf(file, "%s\n", filename);

    // 关闭文件
    fclose(file);
}

// 将 Transaction 转换为 cJSON 对象
cJSON* transactionToJSON(Transaction* transaction)
{
    cJSON* jsonTransaction = cJSON_CreateObject();

    cJSON_AddStringToObject(jsonTransaction, "txid", transaction->txid);
    cJSON_AddNumberToObject(jsonTransaction, "input_count", transaction->input_count);
    cJSON_AddNumberToObject(jsonTransaction, "output_count", transaction->output_count);
    cJSON_AddNumberToObject(jsonTransaction, "is_coinbase", transaction->is_coinbase);

    return jsonTransaction;
}

// 将 Block 转换为 cJSON 对象
cJSON* blockToJSON(Block* block)
{
    cJSON* jsonBlock = cJSON_CreateObject();

    cJSON_AddNumberToObject(jsonBlock, "height", block->height);
    cJSON_AddNumberToObject(jsonBlock, "hash", block->hash);
    cJSON_AddNumberToObject(jsonBlock, "prevHash", block->prevHash);
    cJSON_AddNumberToObject(jsonBlock, "transaction_count", block->transaction_count);

    cJSON* jsonTransactionsArray = cJSON_CreateArray();
    for (int i = 0; i < block->transaction_count; ++i) {
        cJSON* jsonTransaction = transactionToJSON(&block->transactions[i]);
        cJSON_AddItemToArray(jsonTransactionsArray, jsonTransaction);
    }
    cJSON_AddItemToObject(jsonBlock, "transactions", jsonTransactionsArray);

    return jsonBlock;
}

// 将 cJSON 对象写入 JSON 文件的函数
void writeJsonToFile(cJSON *jsonObject, const char *filename)
{
    // 打开文件以进行写入
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file for writing\n");
        exit(EXIT_FAILURE);
    }

    // 将 cJSON 对象写入文件
    char* json_string = cJSON_Print(jsonObject);
    fprintf(file, "%s\n", json_string);
    free(json_string);

    // 关闭文件
    fclose(file);
}

// djb2哈希算法，利用随机数生成一个随机hash
unsigned long hash_djb2(unsigned int num)
{
    unsigned long hash = 5381;

    // 将整数转换为字符串，并使用djb2算法计算哈希值
    char str[20]; // 20 是足够存储整数的字符串表示的缓冲区大小
    sprintf(str, "%u", num);

    int c;
    for (int i = 0; (c = str[i]); ++i) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

// 初始化存储链表，并将需要写入当前区块的的transaction写入一个链表
Node_Transaction* select_Transaction(int number_select)
{
    // 建立一个头结点先
    Node_Transaction* head = (Node_Transaction*)malloc(sizeof(Node_Transaction));
    head->next = 0;
    
    // 开始读取文件插入链表
    int height;
    char txid_str[100];
    char* txid = txid_str;
    int is_coinbase;
    int input_count;
    int output_count;
    FILE* file = fopen("transactions.csv", "r");
    if (file == NULL)
    {
        perror("无法打开文件");
        return 0;
    }
    
    // 给每个交易一个编号，便于查询和删除
    int index = 0;

    // 逐行读取CSV文件
    char line[1024];
    int lineCount = 0;
    while (fgets(line, sizeof(line), file))
    {
        lineCount++;
        if (lineCount == 1)
        {
            continue;
        }
        // 使用strtok函数拆分CSV行
        char* token = strtok(line, ",");
        if (token == NULL) {
            continue; // 忽略空行
        }

        // 解析CSV列数据
        height = atoi(token);
        // printf("%d\n", height);
        token = strtok(NULL, ",");
        txid = token;
        token = strtok(NULL, ",");
        is_coinbase = atoi(token);
        // printf("%d\n", is_coinbase);
        token = strtok(NULL, ",");
        input_count = atoi(token);
        token = strtok(NULL, ",");
        output_count = atoi(token);


        // 给新节点插入数据
        Node_Transaction* Node = (Node_Transaction*)malloc(sizeof(Node_Transaction));
        Node->index = ++index;
        Node->transaction.txid = strdup(txid);
        Node->transaction.input_count = input_count;
        Node->transaction.output_count = output_count;
        Node->transaction.is_coinbase = is_coinbase;

        // 头插
        Node->next = head->next;
        head->next = Node;
        
        if (index == number_select)
        {
            break;
        }
    }

    // 一定要关闭文件
    fclose(file);

    return head;
}

// 初始化存储链表，并将需要写入当前区块的的transaction写入一个链表
Node_Transaction* select_Transaction_generate(int number_select)
{
    // 建立一个头结点先
    Node_Transaction* head = (Node_Transaction*)malloc(sizeof(Node_Transaction));
    head->next = 0;
    
    // 开始读取文件插入链表
    int height;
    char txid_str[100];
    char* txid = txid_str;
    int is_coinbase;
    int input_count;
    int output_count;
    FILE* file = fopen("transactions_generate.csv", "r");
    if (file == NULL)
    {
        perror("无法打开文件");
        return 0;
    }
    
    // 给每个交易一个编号，便于查询和删除
    int index = 0;

    // 逐行读取CSV文件
    char line[1024];
    int lineCount = 0;
    while (fgets(line, sizeof(line), file))
    {
        lineCount++;
        if (lineCount == 1)
        {
            continue;
        }
        // 使用strtok函数拆分CSV行
        char* token = strtok(line, ",");
        if (token == NULL) {
            continue; // 忽略空行
        }

        // 解析CSV列数据
        height = atoi(token);
        // printf("%d\n", height);
        token = strtok(NULL, ",");
        txid = token;
        token = strtok(NULL, ",");
        is_coinbase = atoi(token);
        // printf("%d\n", is_coinbase);
        token = strtok(NULL, ",");
        input_count = atoi(token);
        token = strtok(NULL, ",");
        output_count = atoi(token);


        // 给新节点插入数据
        Node_Transaction* Node = (Node_Transaction*)malloc(sizeof(Node_Transaction));
        Node->index = ++index;
        Node->transaction.txid = strdup(txid);
        Node->transaction.input_count = input_count;
        Node->transaction.output_count = output_count;
        Node->transaction.is_coinbase = is_coinbase;

        // 头插
        Node->next = head->next;
        head->next = Node;
        
        if (index == number_select)
        {
            break;
        }
    }

    // 一定要关闭文件
    fclose(file);

    return head;
}

// 随机选取交易生成一个区块
Block* New_Block(Block* list, Node_Transaction* select)
{
    int transaction_count = select->next->index;

    // 将各类的信息插入block
    Block* block = (Block*)malloc(sizeof(Block));
    srand((unsigned int)time(NULL));
    block->hash = hash_djb2(rand());
    block->prevHash = list->prev->hash;
    block->transaction_count = transaction_count;
    block->height = Height;

    // 尾插
    block->next = list;
    block->prev = list->prev;
    list->prev->next = block;
    list->prev = block;

    // 把交易信息写入block
    Node_Transaction* current = select->next;
    for (int i = transaction_count - 1; i >= 0; i--)
    {
        block->transactions[i] = current->transaction;
        current = current->next;
    }

    return block;
}

// 添加节点过程
Block* Add_Block(Block* list)
{
    Height++;

    // 生成随机选取前n条transaction信息
    srand((unsigned int)time(NULL));
    int number_select = rand() % 5 + 1;

    // 根据transactions生成去区块
    Block* block = New_Block(list, select_Transaction(number_select));
    
    // 将区块信息写成json文件发送，记得释放内存
    cJSON* block_json = blockToJSON(block);
    char filename[20];
    sprintf(filename, "./message2/block_%d.json", block->height);
    writeJsonToFile(block_json, filename);
    cJSON_Delete(block_json);
    writeFilenameToCSV(filename);
    
    // 在transaction池中删除已经生成的信息
    deleteLines("transactions.csv", number_select);

    return block; 
}

// 创建一个区块链
Block* createBlocklist()
{
    // 初始化一个头结点
    Block* head = (Block*)malloc(sizeof(Block));
    head->hash = 0;
    head->height = -1;
    head->prevHash = 0;
    head->transaction_count = 0;
    head->next = head;
    head->prev = head;
    
    return head;
}

// 循环节点逻辑
void loopActivate(Block* list)
{
    printf("print the status\n1 start/continue transaction\n0 exit transaction\n\n");

    // 定义处理状态
    int status;
    // 状态码
    scanf("%d", &status);

    while (status)
    {
        // 生成一个0-1间的随机数
        srand((unsigned int)time(NULL));
        double random_number = rand() % 10;

        // 处理消息队列
        if (random_number < 1)
        {
            // 中奖
            Add_Block(list);
        }
        else
        {
            char* jsonname = getMessage("./message1/block_message1.csv");
            if (strcmp(jsonname, "empty") != 0)
            {
                // 区块消息队列不为空，取出区块
                Block* get_block = readBlockFromJSONFile(jsonname);

                // 尾插
                get_block->next = list;
                get_block->prev = list->prev;
                list->prev->next = get_block;
                list->prev = get_block;
            }
            else
            {
                // 区块消息队列为空，查询客户消息队列
                char* sign = getMessage("./message1/agent_message1.csv");
                if (strcmp(sign, "transaction") == 0)
                {
                    // 处理交易请求
                    Node_Transaction* get_transaction = select_Transaction_generate(1);
                    Transaction transaction = get_transaction->next->transaction;

                    // 打开交易池文件
                    FILE* file = fopen("transactions.csv", "a");
                    // 追加写入Transaction的信息到CSV文件中
                    fprintf(file, "%s,%d,%d,%d\n", transaction.txid, transaction.input_count, transaction.output_count, transaction.is_coinbase);
                    // 关闭文件
                    fclose(file);
                }
                else if (strcmp(sign, "lookup") == 0)
                {
                    // 处理查询请求
                    infoLinkedList(list);
                }
            }
        }

        // 状态码
        scanf("%d", &status);
    }
}

// 获取队头的消息
char* getMessage(const char* filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    // 跳过第一行
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return NULL;  // 无法读取表头
    }

    // 读取第二行
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return "empty";  // 队列为空
    }

    // 关闭文件
    fclose(file);

    // 去掉末尾的换行符
    char *newline = strchr(line, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }

    // 将字符串拷贝到新的动态分配的内存中
    char *message = strdup(line);
    if (message == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    // 消息出队
    deleteLines(filename, 1);
    return message;
}

// 从 cJSON 对象解析并填充 Transaction 结构
void parseTransactionFromJSON(cJSON* jsonTransaction, struct Transaction* transaction)
{
    transaction->txid = strdup(cJSON_GetStringValue(cJSON_GetObjectItem(jsonTransaction, "txid")));
    transaction->input_count = cJSON_GetNumberValue(cJSON_GetObjectItem(jsonTransaction, "input_count"));
    transaction->output_count = cJSON_GetNumberValue(cJSON_GetObjectItem(jsonTransaction, "output_count"));
    transaction->is_coinbase = cJSON_GetNumberValue(cJSON_GetObjectItem(jsonTransaction, "is_coinbase"));
}

// 从 JSON 文件中读取数据并返回 Block 结构
Block* readBlockFromJSONFile(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file for reading.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* jsonString = (char*)malloc(fileSize + 1);
    fread(jsonString, 1, fileSize, file);
    fclose(file);

    jsonString[fileSize] = '\0';

    cJSON* jsonBlock = cJSON_Parse(jsonString);
    free(jsonString);

    if (!jsonBlock)
    {
        printf("Error parsing JSON.\n");
        return NULL;
    }

    struct Block* block = (struct Block*)malloc(sizeof(struct Block));

    // 填充 Block 结构
    block->height = cJSON_GetNumberValue(cJSON_GetObjectItem(jsonBlock, "height"));
    block->hash = (unsigned long)cJSON_GetNumberValue(cJSON_GetObjectItem(jsonBlock, "hash"));
    block->prevHash = (unsigned long)cJSON_GetNumberValue(cJSON_GetObjectItem(jsonBlock, "prevHash"));
    block->transaction_count = cJSON_GetNumberValue(cJSON_GetObjectItem(jsonBlock, "transaction_count"));

    cJSON* jsonTransactionsArray = cJSON_GetObjectItem(jsonBlock, "transactions");
    for (int i = 0; i < cJSON_GetArraySize(jsonTransactionsArray); ++i)
    {
        cJSON* jsonTransaction = cJSON_GetArrayItem(jsonTransactionsArray, i);
        parseTransactionFromJSON(jsonTransaction, &block->transactions[i]);
    }

    // 释放内存
    cJSON_Delete(jsonBlock);
    
    // 删除json文件
    remove(filename);

    block->height = ++Height;
    return block;
}

// 打印链表长度
void lengthLinkedList(Block* list)
{
    int count = 0;
    Block* temp = list;
    while (temp->next != list)
    {
        count++;
        temp = temp->next;
    }

    printf("The length of the block chain is: %d\n", count);
}

// 统计交易数量
int countTransaction(Block* list)
{
    int count = 0;
    Block* temp = list->next;
    while (temp->next != list)
    {
        count += temp->transaction_count;
        temp = temp->next;
    }
    count += temp->transaction_count;

    return count;
}

// 生成list基本信息
void infoLinkedList(Block* list)
{
    printf("\nInformation: \n");
    lengthLinkedList(list);
    int total = countTransaction(list);
    printf("The number of transactions: %d\n", total);
}
