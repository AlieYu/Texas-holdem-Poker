#ifndef _PLAYER_
#define _PLAYER_

#include <vector>
#include <string>
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
extern class Player& Ares; 


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
	NoAction=0,Check=1,Call,Raise,AllIn,Fold //让牌 跟注 加注 全押 弃牌，NoAction表示还未决定状态
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
extern CT_Value GetCT_Value(Card* cards,int num);

//声明两个全局函数
Card* GetPubCards();
int GetCardsNum();


//牌手
class Player{
public:
	Player():seat(-1),CurrPot(0),TotalPot(0),jetton(0),ReMoney(0),CurrAction(NoAction),
		     isContinue(true),name(NULL){   }
	
	CT_Value GetBestCTV(){
		Card* cds=new Card[7];
		cds[0]=HoldCards[0];
		cds[1]=HoldCards[1];
		for(int i=0;i<5;++i)
		{
			cds[i+2]=GetPubCards()[i];
		}
		this->ctv=GetCT_Value(cds, GetCardsNum());
		delete[] cds;
		return this->ctv;
	}						//获取当前最佳牌型	(5 6 7张牌)
	int GetSeat(){return seat;}					//获取作为信息
	int GetTotalPot(){return TotalPot;}			//获取本局累计已投注额
	int Getjetton(){return jetton;}				//获取当前彩池总额
	int GetRemony(){return ReMoney;}			//获取剩余金币数量
	Card* GetHoldCs(){return HoldCards;}		//获取手牌
	vector<int> GetActions(){return actions;}	//获取历史action记录
	int GetCurrAction(){return CurrAction;}		//获取最近一次action
	bool GetisContinue(){return isContinue;}	//获取牌局是否还在继续

	//策略函数1
	//策略函数2
	//策略函数3

public:
	int pid;			//玩家标识
	char* name;			
	int seat;			//座位编号（以庄家的相对位置）
	int CurrPot;		//当前需要的下注额
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
	Game():players(g_PlayerNum),TotalMoney(0),turn(PreHold),RaisePlayer(0),
		  GameOver(false){
		//PubCard=new Card[5];
		AllPlayers=new Player[8];
	}
	~Game(){ 
		delete[] PubCard;
		delete[] AllPlayers;
	}

	Player* GetAllPlalyers(){return AllPlayers;}
	Card* GetPubCards() {return PubCard;}
	int GetPlayers(){return players;}
	int GetTotalMoney(){return TotalMoney;}
	int GetTurn(){return turn;}
	int GetRaisePlayers(){return RaisePlayer;}
	int GetCardsNum() {return CardsNum;}
	bool GetGameOver(){return GameOver;}
	int GetBlindNum(){return BlindsNum;}

public:
	Player* AllPlayers;  //所有玩家
	static Card* PubCard;		//公共牌
	int players;		//当前玩家人数
	int TotalMoney;		//当前底池总量
	int turn;			//当前牌局进展（如翻牌前  翻牌圈 转牌圈 河牌圈）
	int RaisePlayer;    //当前圈加注人数
	static int CardsNum;		//底牌+已发公共牌数量
	bool GameOver;		//游戏结束标志
	int BlindsNum;          //小盲注金额
};




/**********************************************
全局 消息设置函数
*********************************************/

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
bool Set_Msg(const char *buf );


#endif