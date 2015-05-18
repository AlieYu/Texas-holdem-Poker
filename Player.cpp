#include "Player.h"
#include "game.h"
#include <stdlib.h>

/********************************
全局变量
*********************************/
//存放整条消息
char* msgbuff;
//存放将整条消息拆分的结果
VecStr g_MsgVec;

//初始玩家数
const  int g_PlayerNum=8;
//牌局
Game g_game;

/*****************************************************
我是战神Ares
********************************************************/
//pid
int g_Pid;
//name
char* g_name;
//战神的座位号
int g_Seat;   
//初始筹码
const int  g_jetton=1000;
//初始金币
const int g_Money=1000;
//战神 
class Player& Ares=g_game.AllPlayers[g_Seat]; 




/************************************************
Game的静态成员变量
*****************************************/
int Game::CardsNum=0;
Card* Game::PubCard=new Card[5];



/******************************************************
全局函数
****************************************************/

//获取公共牌的全局函数
Card* GetPubCards() {return Game::PubCard;}
//获取底牌+公共牌数量
int GetCardsNum() {return Game::CardsNum;}


//字符串拆分函数：将s按c拆分，每个字段存到lstr_vec
void split(const string& s, char c, VecStr& v)
{
 string::size_type i = 0;
 string::size_type j = s.find(c);
  if (j == string::npos)
  {
    v.push_back(s);
    return;
   }

 while (j != string::npos)
{
 v.push_back(s.substr(i, j - i));
 i = ++j;
 j = s.find(c, j);

 if (j == string::npos)
   {
    string tempStr = s.substr(i, s.length());
    if (tempStr.length() == 0)
       return;
    else
       v.push_back(tempStr);
   }
}
}

//传入消息串，将其按字段拆分
void SplitMsg(const char* buffer, VecStr& g_MsgVec)
{
string tempstr(buffer);
VecStr tempVec;
split(tempstr, '\n', tempVec);
for (int i = 0; i < tempVec.size(); ++i)
 {
  split(tempVec[i], ' ', g_MsgVec);
 }
}
//将pid映射为玩家的座位号
int GetSeatNum(int pid)
{
	for(int i=0;i<8;++i)
		if(g_game.AllPlayers[i].pid==pid)
			return  g_game.AllPlayers[i].seat;
	return -1;
}




/*******************************************
全局函数：服务器消息处理函数   
************************************************/
bool Set_SeatMsg(const VecStr& msg)
{
	if(msg[0]!="seat/" ||msg.size()!=30 || *(msg.end())!="/seat" )
		return false;
	int index=2;
	
	g_game.AllPlayers[0].seat=0;
	g_game.AllPlayers[0].pid=atoi(msg[index].c_str());
	g_game.AllPlayers[0].jetton=atoi(msg[++index].c_str());
	g_game.AllPlayers[0].ReMoney=atoi(msg[++index].c_str());
	
	for(int i=1;i<3;++i)
	{
	g_game.AllPlayers[i].seat=i;
	g_game.AllPlayers[i].pid=atoi(msg[index+=3].c_str());
	g_game.AllPlayers[i].jetton=atoi(msg[++index].c_str());
	g_game.AllPlayers[i].ReMoney=atoi(msg[++index].c_str());
	}
	for(int i=3;i<8;++i)
	{
	g_game.AllPlayers[i].seat=i;
	g_game.AllPlayers[i].pid=atoi(msg[++index].c_str());
	g_game.AllPlayers[i].jetton=atoi(msg[++index].c_str());
	g_game.AllPlayers[i].ReMoney=atoi(msg[++index].c_str());
	}

	//根据我的Pid设置座位号
	for(int i=0;i<8;++i)
		if(g_game.AllPlayers[i].pid==g_Pid)
			g_Seat=g_game.AllPlayers[i].seat;

	return true;
}
bool Set_BlindMsg(const VecStr& msg)
{
	//大于两个玩家
	if(g_game.players>2)
	{
	if(msg[0]!="blind/" || msg.size()!=6 || *(msg.end())!="/blind")
	return false;
	g_game.BlindsNum=atoi(msg[2].c_str());
	g_game.AllPlayers[1].jetton-g_game.BlindsNum;
	g_game.AllPlayers[2].jetton-2*g_game.BlindsNum;
	return true;
	}
	//只有两个玩家
	if(g_game.players==2)
	{
	if(msg[0]!="blind/" || msg.size()!=4 || *(msg.end())!="/blind")
	return false;
	g_game.BlindsNum=atoi(msg[2].c_str());
	g_game.AllPlayers[1].jetton-g_game.BlindsNum;
	return true;
	}
	return false;
}

bool Set_HoldCsMsg(const VecStr& msg)
{
	if(msg.size()!=6 || msg[0]!="hold/" || msg[5]!="/hold" )
		return false;
	
	Ares.HoldCards[0].color=atoi(msg[1].c_str());
	Ares.HoldCards[0].point=atoi(msg[2].c_str());
	Ares.HoldCards[1].color=atoi(msg[3].c_str());
	Ares.HoldCards[1].point=atoi(msg[4].c_str());
	
	g_game.turn=Hold;
	return true;
}

bool Set_InquireMsg(const VecStr& msg)
{
 if(msg[0]!="inquire/" ||*(msg.end())!="/inquire")
	 return false;

 int Index=0;
 //服务器不发送在本轮以前已经弃牌的玩家消息
 for(int i=g_Seat-1;i>=2;--i)//从我的上家逆时针到大盲注的下家
 {
	
	if(g_game.AllPlayers[i].isContinue==true)//玩家在上轮没有弃牌
	{
		++Index;
		g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrPot=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrAction=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//记录玩家的每次决策结果
		if(g_game.AllPlayers[i].CurrAction==Fold)//玩家弃牌
			g_game.AllPlayers[i].isContinue=false;
	
	}
 }
 for(int i=1;i>=0;--i)//大小盲消息
 {
	if(g_game.AllPlayers[i].isContinue==true)//玩家在上轮没有弃牌
	{
		++Index;
		g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrPot=atoi(msg[++Index].c_str());
		if(msg[++Index]!="blind")
			return false;
		//g_game.AllPlayers[i].CurrAction=atoi(msg[++Index].c_str());
		//g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//记录玩家的每次决策结果
		//if(g_game.AllPlayers[i].CurrAction==Fold)//玩家弃牌
			//g_game.AllPlayers[i].isContinue=false;
	
	}
 }
 for(int i=7;i>=g_Seat;--i) //庄家逆时针到我
 {
	 if(g_game.AllPlayers[i].isContinue==true)
	{
		++Index;
		g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrPot=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrAction=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//记录玩家的每次决策结果
		if(g_game.AllPlayers[i].CurrAction==Fold)//玩家弃牌
			g_game.AllPlayers[i].isContinue=false;
	
	}
  
 }
 return true;
}

bool Set_FlopMsg(const VecStr& msg)
{
	if(msg.size()!=8 || msg[0]!="flop/" || msg[7]!="/flop")
		return false;
	int index=0;
	for(int i=0;i<3;++i)
	{
	 g_game.PubCard[i].color=atoi(msg[++index].c_str());
	 g_game.PubCard[i].point=atoi(msg[++index].c_str());
	}
	g_game.turn=Flop;
	return true;
	
}

bool Set_TurnMsg(const VecStr& msg)
{
	if(msg.size()!=4 || msg[0]!="turn/" || msg[3]!="/turn")
		return false;
	 g_game.PubCard[3].color=atoi(msg[1].c_str());
	 g_game.PubCard[3].point=atoi(msg[2].c_str());

	 g_game.turn=Turn;
	 return true;
}

bool Set_RiveMsg(const VecStr& msg)
{
	if(msg.size()!=4 || msg[0]!="turn/" || msg[3]!="/turn")
			return false;
	g_game.PubCard[4].color=atoi(msg[1].c_str());
	g_game.PubCard[4].point=atoi(msg[2].c_str());

	g_game.turn=Rive;
	return true;
}

//摊牌消息
bool Set_ShowDownMsg(const VecStr& msg);
//彩池分配消息
bool Set_PotWinMsg(const VecStr& msg)
{
 if(msg.size()!=4 || msg[0]!="pot-win/" || msg[3]!="/pot-win")
			return false;
 int seat=GetSeatNum(atoi(msg[1].c_str()));
 g_game.AllPlayers[seat].jetton+=atoi(msg[2].c_str());//更新赢家的筹码
 return true;
}
//游戏结束消息
bool Set_GameOver(const VecStr& msg)
{
	 if(msg.size()!=2 || msg[0]!="game-over" )
			return false;
	 g_game.GameOver=true;
	 return true;
 
}
//向服务器注册消息
string Set_RegMsg(int id,char* Name)
{
 string msg;
 Ares.pid=id;
 Ares.name=Name;

 char buf[20];
 sprintf(buf,"%d",id);
 msg=buf;
 msg+=' ';
 msg+=Name;
 msg+=" \n";
 return msg;
}
//战神Ares决策消息   :player->server.   
string Set_ActionMsg(int action,int raisenum)
{
	string msg;

	Ares.CurrAction=action;
	Ares.actions.push_back(action);

	if(action==Check)
	{
		//Ares.jetton=  ;
		msg="check \n";
	}
	else if(action==Fold)
	{
		Ares.isContinue=false;
		msg="fold \n";
	}
	else if(action==Call)
	{
		//Ares.jetton-=   ;
		msg="call \n";
	}
	else if(action==AllIn)
	{
	//	Ares.jetton=   ;
		msg="all_in";	
	}
	else if(action==Raise)
	{
		
		Ares.jetton-=raisenum;
		char buf[10];
		sprintf(buf,"%d",raisenum);
		msg="raise ";
		msg+=buf;
		msg+=" \n";
	}

	return msg;
}

//消息处理接口函数(server -> player)
bool Set_Msg(const char* buf)
{
	bool temp;
	if(buf==NULL)
		return false;

	SplitMsg(buf,g_MsgVec);

	if(g_MsgVec[0]=="seat/")
		temp=Set_SeatMsg(g_MsgVec);
	else if(g_MsgVec[0]=="game-over")
		temp=Set_GameOver(g_MsgVec);
	else if(g_MsgVec[0]=="blind/")
		temp=Set_BlindMsg(g_MsgVec);
	else if(g_MsgVec[0]=="hold/")
		temp=Set_HoldCsMsg(g_MsgVec);
	else if(g_MsgVec[0]=="inquire/")
		temp=Set_InquireMsg(g_MsgVec);
	else if(g_MsgVec[0]=="flop/")
		temp=Set_FlopMsg(g_MsgVec);
	else if(g_MsgVec[0]=="turn/")
		temp=Set_TurnMsg(g_MsgVec);
	else if(g_MsgVec[0]=="river/")
		temp=Set_RiveMsg(g_MsgVec);
	//else if(g_MsgVec[0]=="showdown/")
	//	temp=Set_ShowDownMsg(g_MsgVec);
	else if(g_MsgVec[0]=="pot-win/")
		temp=Set_PotWinMsg(g_MsgVec);

	return temp;
 
}