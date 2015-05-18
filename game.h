#ifndef _GAME_
#define _GAME_

#include <algorithm>
#include "Player.h"

using namespace std;
/**************************************************
全局变量
**************************************************/

//错误标识
extern bool g_invalid;

//extern class Card;
//extern class CT_Value;

//extern class CT_Value;

//牌型值数组
// CT_Value g_CTvalues[7];

//牌数组
//class Card  g_cards[7];


//存放当前已有牌可组成的牌型，每种牌型对应数组的一位，1表示能组成该牌型，否则为不能
//直接以牌型作为下标
extern int isType[9];


/***********************************************************************************
***********************************************************************************/
//排序的比较函数：
//按点数排序
int comp_point(Card a,Card b);

//按花色排序（黑 红 梅 方）
int comp_color(Card a, Card b);

//排序：先按花色排序，同花内部按点数排序    pend为超尾
void CPSort(Card* pbeg,Card* pend);



/*判断给定的 已排序 的五张牌是否能组成某种牌型，若是则设置isType数组对应位为1，
并返回决定牌型的牌，当不是对应类型值时返回始终为point最大的牌*/

//是否为同花顺  
Card isStraightFlush(Card* cards,int beg,int end);

//是否为四条
Card isFourOne(Card* cards,int beg,int end);

//是否为三带二
Card isThreeTwo(Card* cards,int beg,int end);


//是否为同花  传入的5张牌已经按花色排序
Card isFlush(Card* cards,int beg,int end);

//是否为顺子
Card  isStraight(Card* cards,int beg,int end);

//是否为三条
Card isThreeCard(Card* cards,int beg ,int end);
	
//是否为二对
Card isTwoPair(Card* cards,int beg,int end);
	
	//是否为一对
Card isOnePair(Card* cards,int beg,int end);
	
//是否为高牌
Card isHighCard(Card*cards,int beg,int end);
	
	
	
/*当前牌型(最高)（由底牌 和 已有公共牌的得出）有5 或6 或7 张牌*/
CT_Value GetCT_Value(Card* cards,int num);


/*******************************************************************************
创建 四二法则 概率表        先不用！！！！
*************************************************************************************/
//翻牌圈概率
extern double probFlop[50];
//转牌圈概率
extern double probTurn[47];
//河牌圈概率
extern double probRive[46];

//概率表初始化函数
void probInition();
//单圈击中概率查询函数  turn：处于哪圈牌牌 helpnum:可帮助凑成所需牌型的剩余牌张数
double probGet(int turn,int helpnum);




#endif










