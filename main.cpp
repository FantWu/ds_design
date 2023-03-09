// ConnectSix 样例程序
// 随机策略
// 作者：Ryouko
// 游戏信息：http://www.botzone.org/games#ConnectSix


#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "jsoncpp/json.h"


#define GRIDSIZE 15
#define judge_black 0
#define judge_white 1
#define grid_blank 0
#define grid_black 1
#define grid_white -1

using namespace std;

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[GRIDSIZE][GRIDSIZE] = { 0 }; // 先x后y，记录棋盘状态
int dx[] = { -1,-1,-1,0,0,1,1,1 };
int dy[] = { -1,0,1,-1,1,-1,0,1 };

// 判断是否在地图内
inline bool inMap(int x, int y)
{
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return false;
    return true;
}


// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int grid_color, bool check_only)
{
    if (x1 == -1 || y1 == -1) {
        if (!inMap(x0, y0) || gridInfo[x0][y0] != grid_blank)
            return false;
        if (!check_only) {
            gridInfo[x0][y0] = grid_color;
        }
        return true;
    } else {
        if ((!inMap(x0, y0)) || (!inMap(x1, y1)))
            return false;
        if (gridInfo[x0][y0] != grid_blank || gridInfo[x1][y1] != grid_blank)
            return false;
        if (!check_only) {
            gridInfo[x0][y0] = grid_color;
            gridInfo[x1][y1] = grid_color;
        }
        return true;
    }
}

int main()
{
    int x0, y0, x1, y1;

    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    currBotColor = input["requests"][(Json::Value::UInt) 0]["x0"].asInt() < 0 ? grid_black : grid_white; // 第一回合收到坐标是-1, -1，说明我是黑方
    for (int i = 0; i < turnID; i++)
    {
        // 根据这些输入输出逐渐恢复状态到当前回合
        x0 = input["requests"][i]["x0"].asInt();
        y0 = input["requests"][i]["y0"].asInt();
        x1 = input["requests"][i]["x1"].asInt();
        y1 = input["requests"][i]["y1"].asInt();
        if (x0 >= 0)
            ProcStep(x0, y0, x1, y1, -currBotColor, false); // 模拟对方落子
        x0 = input["responses"][i]["x0"].asInt();
        y0 = input["responses"][i]["y0"].asInt();
        x1 = input["responses"][i]["x1"].asInt();
        y1 = input["responses"][i]["y1"].asInt();
        if (x0 >= 0)
            ProcStep(x0, y0, x1, y1, currBotColor, false); // 模拟己方落子
    }

    // 看看自己本回合输入
    x0 = input["requests"][turnID]["x0"].asInt();
    y0 = input["requests"][turnID]["y0"].asInt();
    x1 = input["requests"][turnID]["x1"].asInt();
    y1 = input["requests"][turnID]["y1"].asInt();

    if (x0 >= 0)
        ProcStep(x0, y0, x1, y1, -currBotColor, false); // 模拟对方落子
    int beginPos[GRIDSIZE*GRIDSIZE][2], possiblePos[GRIDSIZE*GRIDSIZE][2];
    bool selfFirstBlack = (turnID == 0 && currBotColor == grid_black);

    int posCount = 0, choice0, choice1;
    for (int i = 0; i < GRIDSIZE; ++i) {
        for (int j = 0; j < GRIDSIZE; ++j) {
            if (gridInfo[i][j] == grid_blank) {
                beginPos[posCount][0] = i;
                beginPos[posCount][1] = j;
                possiblePos[posCount][0] = i;
                possiblePos[posCount++][1] = j;
            }
        }
    }

    // 做出决策（你只需修改以下部分）

    int startX, startY, resultX, resultY;
    if (posCount > 0)
    {
        srand(time(0));
        choice0 = rand() % posCount;
        startX = beginPos[choice0][0];
        startY = beginPos[choice0][1];
        choice1 = rand() % posCount;
        if (selfFirstBlack) {
            resultX = -1;
            resultY = -1;
        } else {
            while (choice0 == choice1) {
                choice1 = rand() % posCount;
            }
            resultX = possiblePos[choice1][0];
            resultY = possiblePos[choice1][1];
        }
    } else {
        startX = -1;
        startY = -1;
        resultX = -1;
        resultY = -1;
    }

    // 决策结束，输出结果（你只需修改以上部分）

    Json::Value ret;
    ret["response"]["x0"] = startX;
    ret["response"]["y0"] = startY;
    ret["response"]["x1"] = resultX;
    ret["response"]["y1"] = resultY;
    ret["debug"]["posCount"] = posCount;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    return 0;
}
