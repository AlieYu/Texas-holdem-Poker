#ifndef _PLAYER_
#define _PLAYER_

#include <vector>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

typedef vector<string> VecStr;

//存放整条消息
extern char* msgbuff;
//存放将整条消息拆分的结果
extern VecStr g_MsgVec;
//初始玩家数
extern const  int g_PlayerNum;
//牌局
extern class Game g_game;

/*****************************************************
我是战神Ares
********************************************************/
//战神的pid
extern int g_Pid;
//战神的name
extern char* g_name;
//战神的座位号
extern int g_Seat;   
//初始筹码
extern const int  g_jetton;
//初始金币
extern const int g_Money;
//战神 
extern class Player* Ares; 


//花色
enum Color{
	NoColor=0,Spade=1,Heart,Club,Diamond  //黑 红 梅 方   NOColor表示花色未知
};

//点数    p0为默认点数，即点数未知
enum Point{
	p0=0,p2=2,p3,p4,p5,p6,p7,p8,p9,p10,pJ,pQ,pK,pA
};

//一张牌
class Card{
public:
	Card():color(NoColor),point(p0){ }
public:
	int color;//花色
	int point;//点数
};

//动作
enum Action{
	NoAction=0,Blind=1,Check,Call,Raise,AllIn,Fold //让牌 跟注 加注 全押 弃牌，NoAction表示还未决定状态
};


//牌型
enum CardType{
	ctHighCard=0,  //高牌 0
		ctOnePair,      //一对1 
		ctTwoPair,	//两对 2 
		ctThreeCard,    //三条 3 
		ctStraight,	//顺子 4
		ctFlush,	//同花  5
		ctThreeTwo,	//三带二  葫芦 6
		ctFourOne,	//四带一  四条 7
		ctStraightFlush //同花顺 8
};

//牌型和其值（如K的同花顺)
class CT_Value{
public:
	CT_Value():type(ctHighCard),value(){ }
public:
	CardType type;
	Card  value;
};

//翻牌  转牌  河牌
enum GameTurn{
	PreHold=0,Hold=1,Flop,Turn,Rive   //PreHold表示牌局还未开始
};

/*当前牌型(最高)（由底牌 和 已有公共牌的得出）有5 或6 或7 张牌*/
//extern CT_Value GetCT_Value(Card* cards,int num);

//声明两个全局函数
Card* GetPubCards();
int GetCardsNum();


//牌手
class Player{
public:
	Player():pid(0),seat(-1),CurrPot(0),TotalPot(0),jetton(0),ReMoney(0),CurrAction(NoAction),
		isContinue(true),CurrTotal(0){   }
	 ~Player();
	 CT_Value GetBestCTV(); //获取当前最佳牌型	(5 6 7张牌)
	 void Inition();   //状态初始化
	 int GetSeat() const {return seat;}				//获取作为信息
	int GetTotalPot() const {return TotalPot;}		//获取本局累计已投注额
	int Getjetton()const {return jetton;}			//获取当前彩池总额
	int GetRemony() const {return ReMoney;}		//获取剩余金币数量
	const Card* GetHoldCs() const {return HoldCards;}	//获取手牌
	const vector<int>& GetActions() const {return actions;}	//获取历史action记录
	int GetCurrAction()const {return CurrAction;}		//获取最近一次action
	bool GetisContinue()const {return isContinue;}	//获取牌局是否还在继续

	//策略函数1
	//策略函数2
	//策略函数3

private:
	int pid;			//玩家标识	
	int seat;			//座位编号（以庄家的相对位置）
	int CurrPot;		//当前最小需要的下注额
	int CurrTotal;      //用于记录Ares当前发牌圈已经投入的总额
	int TotalPot;		//本局累计已投注额
	int jetton;			//手中筹码

	int ReMoney;		//剩余金币数量
	Card HoldCards[2];	//手牌
	vector<int> actions;//保存已发生的每一次的行动决策
	int CurrAction;		//最近（当前）一次action
	bool isContinue;    //标记玩家是否已退出程序,如果已退出，则不再更新其成员消息
	CT_Value ctv;		//当前最佳牌型

};

//牌局
class Game{
public:
	Game():players(0),players_alive(0),TotalMoney(0),turn(PreHold),RaisePla    yer(0),GameOver(false),FirstInq(0),JuShu(0),BlindsNum(0),TurnBegin(0);
	~Game(){ 
		delete[] PubCard;
		delete[] AllPlayers;
	}

      void Inition();   //状态初始化
	const Player* GetAllPlalyers()const {return AllPlayers;}
	const Card* GetPubCards() const {return PubCard;}
	int GetPlayers()const {return players;}
	int GetTotalMoney() const {return TotalMoney;}
	int GetTurn() const {return turn;}
	int GetRaisePlayers() const {return RaisePlayer;}
	int GetCardsNum() const {return CardsNum;}
	bool GetGameOver() const {return GameOver;}
	int GetBlindNum() const {return BlindsNum;}
	int GetJuShu() cosnt {return JuShu;}

private:
	Player* AllPlayers;  //所有玩家
	static Card* PubCard;		//公共牌
	int players;		//当前局玩家人数
        int players_alive;  //当前未弃牌玩家数
	int TotalMoney;		//当前底池总量
	int turn;			//当前牌局进展（如翻牌前  翻牌圈 转牌圈 河牌圈）
	int RaisePlayer;    //当前圈加注人数
	static int CardsNum;		//底牌+已发公共牌数量
	bool GameOver;		//本局游戏结束标志
	int BlindsNum;          //小盲注金额
	int FirstInq; 	//标记第一条询问消息
	int JuShu;		//参加过的总局数
	int TurnBegin;		//标记每次发牌后的第一圈询问的起点
	int LastTotalPot[8];//用于供Ares记录上一次询问消息提供的所有玩家本局的总投注额	
};


//调试输出函数
void PrintPlayer(const Player& p);
void PrintGame(const Game& g);
/**********************************************
全局 消息设置函数
*********************************************/
//将消息中的J Q K A转换为11 12  13 14
 int StrToPoint(const string& c);
 //将消息中的花色转换为color
 int StrToColor(const string& c);
 //将action 转换为int
int StrToAction(const string& c);


int FindMaxPot(int beg,int end);
//字符串拆分函数：将s按c拆分，每个字段存到lstr_vec
void split(const string& s, char c, VecStr& Vec);
//传入消息串，将其按字段拆分
void SplitMsg(char* buffer, VecStr& msg);
//设置坐位信息
bool Set_SeatMsg(const VecStr& msg);
//设置大小盲注
bool Set_BlindMsg(const VecStr& msg);
//设置手牌
bool Set_HoldCsMsg(const VecStr& msg);
//设置询问消息
bool Set_InquireMsg(const VecStr& msg);
//设置翻牌
bool Set_FlopMsg(const VecStr& msg);
//设置转牌
bool Set_TurnMsg(const VecStr& gmsg);
//设置河牌
bool Set_RiveMsg(const VecStr& msg);
//设置摊牌结果
bool Set_ShowDownMsg(const VecStr& msg);
//设置彩池分配结果
bool Set_PotWinMsg(const VecStr& msg);
//设置gameover标志
bool Set_GameOver(const VecStr& msg);
//设置action消息：player->server
string Set_ActionMsg(int action,int raisenum=0);
//设置注册消息：payer->server
string Set_RegMsg(int id,char* Name);

//消息处理调用接口函数
bool Set_Msg(const char *buf ,int socket);


#endif
