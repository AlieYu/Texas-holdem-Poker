#include "Player.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>


/********************************
全局变量
*********************************/
//存放整条消息
char* msgbuff;
//存放将整条消息拆分的结果
VecStr g_MsgVec;
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
//赢牌局数
int g_WinTimes=0;
//初始筹码
const int  g_jetton=1000;
//初始金币
const int g_Money=1000;
//战神 
class Player* Ares;//=g_game.AllPlayers[g_Seat]; 
//记录本局当前发牌圈开始时Ares已经投入的总额
int g_TempPot=0;



/************************************************
Game的静态成员变量   成员函数
*****************************************/
int Game::CardsNum=0;
Card* Game::PubCard=new Card[5];

 void Player::Inition()
 {
	 //手牌
HoldCards[0].color=NoColor;
HoldCards[0].point=p0;
HoldCards[1].color=NoColor;
HoldCards[1].point=p0;
 pid=0;
 seat=-1;
 CurrPot=0;
 TotalPot=0;
 jetton=0;
 ReMoney=0;
 CurrAction=NoAction;
 isContinue=true;
 CurrTotal=0;
 }
 void Game::Inition()
 {
	 //JuShu不清零
	g_game.FirstInq=0;
	players=0;
	players_alive=0;
	TotalMoney=0;
	turn=PreHold;
	TurnBegin=0;
	//公共牌
	for(int i=0;i<5;++i)
	{
		PubCard[i].color=NoColor;
		PubCard[i].point=p0;
	}
	//盲注
	BlindsNum=0;
	RaisePlayer=0;
	CardsNum=0;
	GameOver=false;
	for(int i=0;i<8;++i)
		{
			Game::AllPlayers[i].Inition();
			LastTotalPot[i]=0;
		}
 }
		    
CT_Value Player::GetBestCTV()
{
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


//调试输出函数
void PrintPlayer(const Player& p)
{
	cout<<"Player: "<<"pid:"<<p.pid<<" "<<"seat:"<<p.seat<<" "<<"jetton:"<<p.jetton
		<<" "<<"money:"<<p.ReMoney<<" "<<"TotalPot:"<<p.TotalPot<<" "<<"CurrPot:"
		<<p.CurrPot<<" "<<"currAction"<<p.CurrAction<<" "<<"cd1_color:"
		<<p.HoldCards[0].color<<" "<<"cd1_point:"<<p.HoldCards[0].point<<" "
		<<"cd2_color:"<<p.HoldCards[1].color<<" "<<"cd2_point:"
		<<p.HoldCards[1].point<<endl;

}
void PrintGame(const Game& g)
{
	cout<<endl;
	cout<<"JuShu: "<<g_game.JuShu<<endl;
	cout<<"g_seat:"<<g_Seat<<"g_Pid:"<<g_Pid<<endl;

	cout<<"Gmae: "<<"palyer_num:"<<g_game.players<<" "<<"cds_num:"<<g_game.CardsNum<<" "
		<<"turn:"<<g_game.turn<<" "<<"BlindNum:"<<g_game.BlindsNum<<" "<<"TotalMoney:"
		<<g_game.TotalMoney<<" "<<"GameOver:"<<g_game.GameOver<<endl;
	cout<<"Public Cards:"<<endl;
	for(int i=0;i<g_game.CardsNum-2;++i)
		cout<<"color:"<<g_game.PubCard[i].color<<" point:"<<g_game.PubCard[i].point<<endl;
	for(int i=0;i<8;i++)
		PrintPlayer(g_game.AllPlayers[i]);
	if(g_game.CardsNum>=5)
		cout<<"Best CTV.type: "<<Ares->GetBestCTV().type<<
		" Best CTV.value.point: "<<Ares->GetBestCTV().value.point<<endl;
	cout<<"My win times: "<<g_WinTimes<<endl;
	cout<<"Ares->CurrPot:"<<Ares->CurrPot<<"   Ares->CurrTotal: "<<Ares->CurrTotal<<endl;
}
int FindMaxPot(int beg,int end)
{
	int j=0;
	int max=0;
	for(int i=beg;i>=end;--i)
	{
	 (i>=0)?(j=i):(j=(g_game.players+i));
	 if(g_game.AllPlayers[j].TotalPot >max)
		 max=g_game.AllPlayers[j].TotalPot; 

	}
	return max;
}
/******************************************************
全局函数 辅助函数
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
g_MsgVec.clear();
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

//将2-9  J Q K A转换为p0 - p14
 int StrToPoint(const string& c)
 {
	int point=0;
  if(c=="J")
	  point=pJ;
  else if(c=="Q")
	  point=pQ;
  else if(c=="K")
	  point=pK;
  else if (c=="A")
	  point=pA;
  else point=atoi(c.c_str());
  return point;

 }
 //将花色转换为int
int StrToColor(const string& c)
{
	
 int color=0;
  if(c=="SPADES")
	  color=Spade;
  else if(c=="HEARTS")
	 color=Heart;
  else if(c=="CLUBS")
	  color=Club;
  else if (c=="DIAMONDS")
	 color=Diamond;
  else
	  color=NoColor;
  return color;
}
//将action 转换为int
int StrToAction(const string& c)
{
	int action=NoAction;
  if(c=="check")
	  action=Check;
  else if(c=="call")
	  action=Call;
  else if(c=="raise")
	  action=Raise;
  else if (c=="all_in")
	  action=AllIn;
  else if(c=="fold")
	  action=Fold;
  else if(c=="blind")
	  action=Blind;
  return action;
}


 /*******************************************
全局函数：服务器消息处理函数   
************************************************/
bool Set_SeatMsg(const VecStr& msg)
{
	if(msg[0]!="seat/" || *(msg.end()-1)!="/seat" )
		return false;
	int index=2;
	int length=msg.size();
	//玩家 个数需要判断
	if(length<11)
	{
		g_game.players=1;
		return false;
	}
	else if(length==11)//只剩庄家 和小盲注两个玩家
		g_game.players=2;
	else //至少有三个玩家
		g_game.players=3+(length-16)/3;
	//初始化当前未弃牌玩家数
	g_game.players_alive=g_game.players;
	
	g_game.AllPlayers[0].seat=0;
	g_game.AllPlayers[0].pid=atoi(msg[index].c_str());
	g_game.AllPlayers[0].jetton=atoi(msg[++index].c_str());
	g_game.AllPlayers[0].ReMoney=atoi(msg[++index].c_str());
	
	for(int i=1;i<3 && i<g_game.players;++i)
	{
	g_game.AllPlayers[i].seat=i;
	g_game.AllPlayers[i].pid=atoi(msg[index+=3].c_str());
	g_game.AllPlayers[i].jetton=atoi(msg[++index].c_str());
	g_game.AllPlayers[i].ReMoney=atoi(msg[++index].c_str());
	}
	for(int i=3;i<g_game.players;++i)
	{
	g_game.AllPlayers[i].seat=i;
	g_game.AllPlayers[i].pid=atoi(msg[++index].c_str());
	g_game.AllPlayers[i].jetton=atoi(msg[++index].c_str());
	g_game.AllPlayers[i].ReMoney=atoi(msg[++index].c_str());
	}

	//根据我的Pid设置座位号
	for(int i=0;i<g_game.players;++i)
		if(g_game.AllPlayers[i].pid==g_Pid)
			{
				g_Seat=g_game.AllPlayers[i].seat;
				Ares=&(g_game.AllPlayers[g_Seat]);
				++g_game.JuShu;//Ares参加过的局数
			}

	return true;
}
bool Set_BlindMsg(const VecStr& msg)
{

	if(msg[0]!="blind/" || *(msg.end()-1)!="/blind")
		return false;
	//大于两个玩家
	if(g_game.players>2)
	{
	g_game.BlindsNum=atoi(msg[2].c_str());
	g_game.AllPlayers[1].jetton-=g_game.BlindsNum;
	g_game.AllPlayers[1].TotalPot+=g_game.BlindsNum;
	g_game.AllPlayers[2].jetton-=g_game.BlindsNum<<1;
	g_game.AllPlayers[2].TotalPot+=g_game.BlindsNum<<1;
	g_game.TotalMoney=3*g_game.BlindsNum;
	return true;
	}
	//只有两个玩家
	if(g_game.players==2)
	{
	g_game.BlindsNum=atoi(msg[2].c_str());
	g_game.AllPlayers[1].jetton-=g_game.BlindsNum;
	g_game.AllPlayers[1].TotalPot+=g_game.BlindsNum;
	g_game.TotalMoney=g_game.BlindsNum;
	return true;
	}
	return false;
}

bool Set_HoldCsMsg(const VecStr& msg)
{
	if(msg[0]!="hold/" || *(msg.end()-1)!="/hold" )
		return false;
	 
	Ares->HoldCards[0].color=StrToColor(msg[1]);
	Ares->HoldCards[0].point=StrToPoint(msg[2]);
	Ares->HoldCards[1].color=StrToColor(msg[3]);
	Ares->HoldCards[1].point=StrToPoint(msg[4]);
	Ares->CurrTotal=0;
	g_game.turn=Hold;
	g_game.CardsNum=2;
	g_game.TurnBegin=1;//开始进入手牌圈

	return true;
}

bool Set_InquireMsg(const VecStr& msg)
{
 if(msg[0]!="inquire/" ||*(msg.end()-1)!="/inquire")
	 return false;
 ++g_game.FirstInq;
 int Index=0;
 
 int end=g_Seat-g_game.players;
 int beg=g_Seat-1;

 //询问消息的条数根据players的玩家数一样
 for(int i=beg;i>=end;--i)
 {
	if(i>=0)//从我的上家逆时针到庄家
	{
	
			++Index;
			g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
			g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
			//记录该玩家上次的总投注额
			g_game.LastTotalPot[i]=g_game.AllPlayers[i].TotalPot;
			g_game.AllPlayers[i].TotalPot=atoi(msg[++Index].c_str());
			g_game.AllPlayers[i].CurrAction=StrToAction(msg[++Index]);
		
			g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//记录玩家的每次决策结果
			if(g_game.AllPlayers[i].CurrAction==Fold)//玩家弃牌
				{
					g_game.AllPlayers[i].isContinue=false;
					--g_game.players_alive;
				}

		if(g_game.FirstInq==1 && i==1 && g_Seat!=2)//第一条询问消息 只包含到小盲注为止(但大盲注需要逆时针穿过小盲注回到自己)
			break;
	
	}
	else if(i<0) //庄家逆时针到我
	{
		int j=g_game.players+i;
		if(g_game.FirstInq==1 && g_Seat==0 && j==0)  //Ares是庄家
			break;

			++Index;
			g_game.AllPlayers[j].jetton=atoi(msg[++Index].c_str());
			g_game.AllPlayers[j].ReMoney=atoi(msg[++Index].c_str());
			//记录该玩家上次的总投注额
			g_game.LastTotalPot[j]=g_game.AllPlayers[j].TotalPot;
			g_game.AllPlayers[j].TotalPot=atoi(msg[++Index].c_str());
			g_game.AllPlayers[j].CurrAction=StrToAction(msg[++Index]);
			g_game.AllPlayers[j].actions.push_back(g_game.AllPlayers[j].CurrAction);//记录玩家的每次决策结果
			if(g_game.AllPlayers[j].CurrAction==Fold)//玩家弃牌
				g_game.AllPlayers[j].isContinue=false;
	
	}	
 }
  g_game.TotalMoney=atoi(msg[Index+3].c_str());	
  //计算Ares当前发牌圈已经投入的总额
  if(g_game.TurnBegin==1)
  {
   Ares->CurrTotal=0;
   g_TempPot=Ares->TotalPot;
  }	 
  else 
  {
	  Ares->CurrTotal=Ares->TotalPot-g_TempPot;
  }
 //计算Ares本轮需要的最小投注额
 int temp=0;
 //Ares->CurrPot=temp;
 if(g_game.players==2 && g_game.TurnBegin==1 && g_game.turn==Hold && g_Seat==0)//只有两个玩家
 {
//	if(g_game.TurnBegin==1 && g_game.turn==Hold && g_Seat==0)//新手牌牌圈
	//{
		 g_game.TurnBegin=0;
		temp=g_game.BlindsNum<<1;
	//}
 }
 else //至少三个玩家
 {
	 g_game.TurnBegin=0;
	 int max=FindMaxPot(beg,end);
	 temp=max-Ares->TotalPot;

 }
 Ares->CurrPot=temp;
 /*
 if(g_game.TurnBegin!=1)//不为新的发牌圈
 {
  for(int i=beg;i>=end;--i)
  {
	 if(i>=0)
		{
			if((temp=g_game.AllPlayers[i].TotalPot-g_game.LastTotalPot[i]) > 0)
			 break;
	    }
	 else if(i<0)
		{
			int j=g_game.players+i;
			if((temp=g_game.AllPlayers[j].TotalPot-g_game.LastTotalPot[j]) > 0)
				break;
		}	 
  }
 }
 else//为新的发牌圈
 {   
	 g_game.TurnBegin=0;
	 if(g_game.players>=3)
	 {
	  if(g_game.turn==Hold && g_Seat==3 )//Ares手牌圈的发话者(2个玩家以上)
		 temp=g_game.BlindsNum<<1;
	  else if(g_game.turn==Hold && g_Seat!=3 )//Ares不是手牌圈的发话者
	  {
		     int i=0;
		      for(int i=beg;i>=end;--i)  //非手牌圈本由小盲注发话
			  {
				  int j=i;
				  if(j<0)
					 j=g_game.players+i;
			  	if(g_game.AllPlayers[j].CurrAction!=Fold)
				 {
				  temp=g_game.AllPlayers[j].TotalPot-g_game.LastTotalPot[j];
					  break;
				 }
			 if(j==3)//Ares前面的玩家都已经弃牌，有Ares发话(正常不会运行到这里！！)
				{
					//cout<<"??????????????正常不会运行到这里！！????????????"<<endl;
				temp=g_game.BlindsNum<<1;  //至少为2倍小盲注？?
				break;
				}
			 }
	  }
	  else if(g_game.turn!=Hold) 	//非手牌圈，Ares是发话者
		 {
			
			 int i=0;
		     for(int i=beg;i>=end;--i)  //非手牌圈本由小盲注发话
			 {
				  int j=i;
				  if(j<0)
					 j=g_game.players+i;
				 if(g_game.AllPlayers[j].CurrAction!=Fold)
				 {
				  temp=g_game.AllPlayers[j].TotalPot-g_game.LastTotalPot[j];
					  break;
				 }
				 if(j==1)//Ares前面的玩家都已经弃牌，有Ares发话
					{
					temp=g_game.BlindsNum<<1;  //至少为2倍小盲注？?
				    break;	
				 }			 
			 }
		 }	 
	 }
	 else if(g_game.players==2 && g_Seat==0)//Ares手牌圈的发话者(2个玩家)
		 temp=g_game.AllPlayers[1].TotalPot-g_game.LastTotalPot[1];	
 }
 */
// Ares->CurrPot=temp;
 return true;

}

bool Set_FlopMsg(const VecStr& msg)
{
	if(msg[0]!="flop/" || *(msg.end()-1)!="/flop")
		return false;
	int index=0;
	for(int i=0;i<3;++i)
	{
	 g_game.PubCard[i].color=StrToColor(msg[++index]);
	 g_game.PubCard[i].point=StrToPoint(msg[++index]);
	}
	g_game.turn=Flop;
	g_game.CardsNum+=3;
	g_game.TurnBegin=1;//开始进入翻牌圈
	Ares->CurrTotal=0;
	return true;
	
}

bool Set_TurnMsg(const VecStr& msg)
{
	if(msg[0]!="turn/" || *(msg.end()-1)!="/turn")
		return false;
	 g_game.PubCard[3].color=StrToColor(msg[1]);
	 g_game.PubCard[3].point=StrToPoint(msg[2]);

	 g_game.turn=Turn;
	 g_game.CardsNum+=1;
	 g_game.TurnBegin=1;//开始进入转牌圈
	 Ares->CurrTotal=0;


	 return true;
}

bool Set_RiveMsg(const VecStr& msg)
{
	if( msg[0]!="river/" || *(msg.end()-1)!="/river")
			return false;
	g_game.PubCard[4].color=StrToColor(msg[1]);
	g_game.PubCard[4].point=StrToPoint(msg[2]);

	g_game.turn=Rive;
	g_game.CardsNum+=1;
	g_game.TurnBegin=1;//开始进入河牌圈
	Ares->CurrTotal=0;

	return true;
}

//摊牌消息
bool Set_ShowDownMsg(const VecStr& msg);
//彩池分配消息
bool Set_PotWinMsg(const VecStr& msg)
{
 if( msg[0]!="pot-win/" || *(msg.end()-1)!="/pot-win")
			return false;
 int seat=GetSeatNum(atoi(msg[1].c_str()));
 g_game.AllPlayers[seat].jetton+=atoi(msg[2].c_str());//更新赢家的筹码
 if(seat==g_Seat)   //记录Ares赢牌局数
	 g_WinTimes++;

 //本局游戏结束  恢复初始状态
// g_game.Inition();
 return true;
}
//本场游戏结束消息
bool Set_GameOver(const VecStr& msg)
{
	 if(msg[0]!="game-over" )
			return false;
	 g_game.GameOver=true;

//本局游戏结束  恢复初始状态
 g_game.Inition();
	 return true;
 
}
//向服务器注册消息
string Set_RegMsg(int id,char* Name)
{
 string msg;
 //Ares->pid=id;
 //Ares->name=Name;

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

	Ares->CurrAction=action;
	//Ares->actions.push_back(action);

	if(action==Check)
	{
		msg="check \n";
	}
	else if(action==Fold)
	{
		msg="fold \n";
	}
	else if(action==Call)
	{
		msg="call \n";
	}
	else if(action==AllIn)
	{
		msg="all_in";	
	}
	else if(action==Raise)
	{
		
		Ares->jetton-=raisenum;
		char buf[10];
		sprintf(buf,"%d",raisenum);
		msg="raise ";
		msg+=buf;
		msg+=" \n";
	}

	return msg;
}

//消息处理接口函数(server -> player)
bool Set_Msg(const char* buf,int s)
{
    bool temp;
	if(buf==NULL)
		return false;

	SplitMsg(buf,g_MsgVec);

	if(g_MsgVec[0]=="seat/")
		{
			 printf("******************seat******************\n");
            temp=Set_SeatMsg(g_MsgVec);
           
        }
	else if(g_MsgVec[0]=="game-over")
           {
        printf("******************GameOver******************\n");
		temp=Set_GameOver(g_MsgVec);
           }
	else if(g_MsgVec[0]=="blind/")
        {
        printf("******************blind******************\n");
		temp=Set_BlindMsg(g_MsgVec);
        }
	else if(g_MsgVec[0]=="hold/")
        {
			printf("******************hold******************\n");
		temp=Set_HoldCsMsg(g_MsgVec);
        }
	else if(g_MsgVec[0]=="inquire/")
    {
         printf("******************inquire******************\n");
        string str = "call \n";
		temp=Set_InquireMsg(g_MsgVec);
       // while (send(s, str.c_str(), str.length(), 0) < 0)
      //  {
       //     printf("send fail!\n");
       // }
    
    }
	else if(g_MsgVec[0]=="flop/")
        {
        printf("******************flop******************\n");
		temp=Set_FlopMsg(g_MsgVec);
        }
	else if(g_MsgVec[0]=="turn/")
        {
        printf("******************turn******************\n");
		temp=Set_TurnMsg(g_MsgVec);
        }
	else if(g_MsgVec[0]=="river/")
        {
        printf("******************river******************\n");
		temp=Set_RiveMsg(g_MsgVec);
        }
	else if(g_MsgVec[0]=="showdown/")
        { 
        printf("******************showdown******************\n");
	//	temp=Set_ShowDownMsg(g_MsgVec);
            	}
	else if(g_MsgVec[0]=="pot-win/")
         {
        printf("******************pot-win******************\n");
		temp=Set_PotWinMsg(g_MsgVec);
         }

	PrintGame(g_game);

	if(g_MsgVec[0]=="pot-win/")
		g_game.Inition();

	return temp;
}
