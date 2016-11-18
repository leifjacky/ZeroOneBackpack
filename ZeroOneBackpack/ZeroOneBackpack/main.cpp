#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <ctime>
#include <Windows.h>
using namespace std;

const int MAX_ITEMS_NUM = 1005;
const int MAX_WEIGHT = 10005;
const int MOD = 10000;

int currentAlgorithm = 0;

struct Item {
	int id, weight, value;

	Item(){}
	Item(int i, int w, int v) :id(i), weight(w), value(v) {}
};

int dpOneDim[MAX_WEIGHT];
int dp[MAX_ITEMS_NUM][MAX_WEIGHT];
int itemsNum, maxWeight;
vector<Item> items;

double startTime, endTime, costTime;
double avgTimeForce, avgTimeDP, avgTimeDPOneDim;
int maxValue;
vector<Item> selectedItems;

bool noPrintSolution = false;

void initialize() {
	items.clear();
	items.push_back(Item());
	memset(dp, 0, sizeof(dp));
}

void DPSolution() {
	selectedItems.clear();
	int currentWeight = maxWeight;
	for (int i = itemsNum; i > 0 && currentWeight > 0; i--) {
		if (dp[i][currentWeight] == dp[i - 1][currentWeight - items[i].weight] + items[i].value){
			selectedItems.push_back(items[i]);
			currentWeight -= items[i].weight;
		}
	}
}

int runDP() {
	startTime = GetTickCount();

	for (int i = 1; i <= itemsNum; i++) {
		for (int j = 0; j < items[i].weight; j++)
			dp[i][j] = dp[i - 1][j];
		for (int j = items[i].weight; j <= maxWeight; j++)
			dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - items[i].weight] + items[i].value);
	}
	maxValue = dp[itemsNum][maxWeight];
	DPSolution();

	endTime = GetTickCount();
	costTime = endTime - startTime;

	return maxValue;
}

int runDPOneDim() {
	startTime = GetTickCount();

	memset(dpOneDim, 0, sizeof(dpOneDim));
	int leftTotalWeight = 0;
	for (int i = 1; i <= itemsNum; i++)
		leftTotalWeight += items[i].weight;
	for (int i = 1; i <= itemsNum; i++) {
		leftTotalWeight -= items[i].weight;
		int bound = max(items[i].weight, maxWeight - leftTotalWeight);
		for (int j = maxWeight; j >= bound; j--)
			dpOneDim[j] = max(dpOneDim[j], dpOneDim[j - items[i].weight] + items[i].value);
	}
	maxValue = dpOneDim[maxWeight];

	endTime = GetTickCount();
	costTime = endTime - startTime;

	return maxValue;
}

void forceNewSolution(int solution) {
	selectedItems.clear();
	for (int i = 1; i <= itemsNum && solution; i++) {
		if (solution & 1)
			selectedItems.push_back(items[i]);
		solution >>= 1;
	}
}

int runForce() {
	startTime = GetTickCount();

	maxValue = 0;
	int allSolution = 1 << itemsNum;
	for (int i = 0; i < allSolution; i++) {
		int currentSolution = i, currentWeight = 0, currentValue = 0;
		for (int j = 1; j <= itemsNum && currentSolution; j++) {
			if (currentSolution & 1) {
				currentWeight += items[j].weight;
				currentValue += items[j].value;
			}
			if (currentWeight > maxWeight)
				break;
			currentSolution >>= 1;
		}
		if (currentWeight <= maxWeight && currentValue > maxValue) {
			maxValue = currentValue;
			forceNewSolution(i);
		}
	}

	endTime = GetTickCount();
	costTime = endTime - startTime;

	return maxValue;
}

int dataType = 0, algorithmType = 0, testNum = 0;

void inputType() {
	printf("01背包问题\n");
	printf("1.手动输入数据\n");
	printf("2.自动生成数据\n");
	printf("请选择数据生成模式：");
	scanf("%d", &dataType);
	printf("\n");
	
	printf("01背包算法\n");
	printf("1.蛮力法测试\n");
	printf("2.动态规划测试\n");
	printf("3.蛮力法与动态规划对比\n");
	printf("4.动态规划测试(一维数组+常数优化)\n");
	printf("5.两种动态规划测试\n");
	printf("请选择算法：");
	scanf("%d", &algorithmType);
	printf("\n");

	printf("请输入实验次数：");
	scanf("%d", &testNum);
	printf("\n");

	printf("请输入背包容量：");
	scanf("%d", &maxWeight);
	printf("\n");

	printf("请输入物品个数：");
	scanf("%d", &itemsNum);
	printf("\n");
}

void generateData() {
	int weight, value;

	initialize();
	if (dataType == 1) {
		for (int i = 1; i <= itemsNum; i++) {
			printf("请输入第%d件物品的容量：", i);
			scanf("%d", &weight);
			printf("请输入第%d件物品的价值：", i);
			scanf("%d", &value);
			printf("\n");

			items.push_back(Item(i, weight, value));
		}
	} else {
		for (int i = 1; i <= itemsNum; i++) {
			int WEIGHT_MOD = (maxWeight / itemsNum + 1) * (rand() % 5 + 1);
			int VALUE_MOD = maxWeight * 2;
			weight = (rand() % MOD*MOD + rand() % MOD) % WEIGHT_MOD + 1;
			value = (rand() % MOD*MOD + rand() % MOD) % VALUE_MOD;
			items.push_back(Item(i, weight, value));
		}
	}
}

void printDescription(int No) {
	printf("第%d组 背包容量：%d\t物品数量：%d\n", No, maxWeight, itemsNum);
	if (itemsNum <= 10)
		for (int i = 1; i <= itemsNum; i++)
			printf("物品%d\t所需容量：%d\t价值：%d\n", i, items[i].weight, items[i].value);
	printf("\n");
}

void printSolution() {
	if (noPrintSolution)
		return;
	if (currentAlgorithm == 1)
		printf("蛮力法\n");
	else if (currentAlgorithm == 2)
		printf("动态规划法\n");
	else if (currentAlgorithm == 3)
		printf("动态规划法(一维数组)\n");
	printf("最大价值：%d\t选择物品数量：%d\t耗时：%.3f ms\n", maxValue, selectedItems.size(), costTime);
	if (itemsNum <= 10 && currentAlgorithm != 3)
		for (int i = 0; i < selectedItems.size(); i++)
			printf("物品%d\t所需容量：%d\t价值：%d\n", selectedItems[i].id, selectedItems[i].weight, selectedItems[i].value);
	printf("\n");
}

void runAlgorithm() {
	switch (algorithmType) {
	case 1:
		currentAlgorithm = 1;
		runForce();
		avgTimeForce += costTime;
		printSolution();
		break;

	case 2:
		currentAlgorithm = 2;
		runDP();
		avgTimeDP += costTime;
		printSolution();
		break;

	case 3:
		currentAlgorithm = 1;
		runForce();
		avgTimeForce += costTime;
		printSolution();

		currentAlgorithm = 2;
		runDP();
		avgTimeDP += costTime;
		printSolution();
		break;

	case 4:
		selectedItems.clear();
		currentAlgorithm = 3;
		runDPOneDim();
		avgTimeDPOneDim += costTime;
		printSolution();
		break;

	case 5:
		currentAlgorithm = 2;
		runDP();
		avgTimeDP += costTime;
		printSolution();

		selectedItems.clear();
		currentAlgorithm = 3;
		runDPOneDim();
		avgTimeDPOneDim += costTime;
		printSolution();

		break;
	}

	if (noPrintSolution)
		return;

	printf("\n\n\n");
}

void outputAvgTime() {
	avgTimeForce /= testNum;
	avgTimeDP /= testNum;
	avgTimeDPOneDim /= testNum;
	
	switch(algorithmType){
	case 1:
		printf("蛮力法平均耗时：%.3f ms\n", avgTimeForce);
		break;
	case 2:
		printf("动态规划法平均耗时：%.3f ms\n", avgTimeDP);
		break;
	case 3:
		printf("蛮力法平均耗时：%.3f ms\t动态规划法平均耗时：%.3f ms\n", avgTimeForce, avgTimeDP);
		break;
	case 4:
		printf("动态规划法(一维数组)平均耗时：%.3f ms\n", avgTimeDPOneDim);
		break;
	case 5:
		printf("动态规划法平均耗时：%.3f ms\t动态规划法(一维数组)平均耗时：%.3f ms\n", avgTimeDP, avgTimeDPOneDim);
		break;
	}
}

void backgroundTest() {
	noPrintSolution = true;

	dataType = 2;
	algorithmType = 5;
	testNum = 20;
	while (scanf("%d%d", &maxWeight, &itemsNum) != EOF) {
	//for (int j=1;j<=10;j++){
		//maxWeight = 10000;
		//itemsNum = j;
		for (int i = 1; i <= testNum; i++) {
			generateData();
			runAlgorithm();
		}
		printf("背包容量：%d\t物品数量%d\n", maxWeight, itemsNum);
		outputAvgTime();
		printf("\n\n\n");
	}
}

int main() {
	//backgroundTest();

	while (1) {
		srand(time(NULL));
		avgTimeDP = avgTimeForce = avgTimeDPOneDim = 0;

		inputType();
		for (int i = 1; i <= testNum; i++) {
			generateData();
			printDescription(i);

			runAlgorithm();
		}

		outputAvgTime();
		printf("\n\n\n");
	}

	return 0;
}