#include "Player.h"
#include "game.h"
#include <stdlib.h>

/********************************
ȫ�ֱ���
*********************************/
//���������Ϣ
char* msgbuff;
//��Ž�������Ϣ��ֵĽ��
VecStr g_MsgVec;

//��ʼ�����
const  int g_PlayerNum=8;
//�ƾ�
Game g_game;

/*****************************************************
����ս��Ares
********************************************************/
//pid
int g_Pid;
//name
char* g_name;
//ս�����λ��
int g_Seat;   
//��ʼ����
const int  g_jetton=1000;
//��ʼ���
const int g_Money=1000;
//ս�� 
class Player& Ares=g_game.AllPlayers[g_Seat]; 




/************************************************
Game�ľ�̬��Ա����
*****************************************/
int Game::CardsNum=0;
Card* Game::PubCard=new Card[5];



/******************************************************
ȫ�ֺ���
****************************************************/

//��ȡ�����Ƶ�ȫ�ֺ���
Card* GetPubCards() {return Game::PubCard;}
//��ȡ����+����������
int GetCardsNum() {return Game::CardsNum;}


//�ַ�����ֺ�������s��c��֣�ÿ���ֶδ浽lstr_vec
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

//������Ϣ�������䰴�ֶβ��
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
//��pidӳ��Ϊ��ҵ���λ��
int GetSeatNum(int pid)
{
	for(int i=0;i<8;++i)
		if(g_game.AllPlayers[i].pid==pid)
			return  g_game.AllPlayers[i].seat;
	return -1;
}




/*******************************************
ȫ�ֺ�������������Ϣ������   
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

	//�����ҵ�Pid������λ��
	for(int i=0;i<8;++i)
		if(g_game.AllPlayers[i].pid==g_Pid)
			g_Seat=g_game.AllPlayers[i].seat;

	return true;
}
bool Set_BlindMsg(const VecStr& msg)
{
	//�����������
	if(g_game.players>2)
	{
	if(msg[0]!="blind/" || msg.size()!=6 || *(msg.end())!="/blind")
	return false;
	g_game.BlindsNum=atoi(msg[2].c_str());
	g_game.AllPlayers[1].jetton-g_game.BlindsNum;
	g_game.AllPlayers[2].jetton-2*g_game.BlindsNum;
	return true;
	}
	//ֻ���������
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
 //�������������ڱ�����ǰ�Ѿ����Ƶ������Ϣ
 for(int i=g_Seat-1;i>=2;--i)//���ҵ��ϼ���ʱ�뵽��äע���¼�
 {
	
	if(g_game.AllPlayers[i].isContinue==true)//���������û������
	{
		++Index;
		g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrPot=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrAction=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//��¼��ҵ�ÿ�ξ��߽��
		if(g_game.AllPlayers[i].CurrAction==Fold)//�������
			g_game.AllPlayers[i].isContinue=false;
	
	}
 }
 for(int i=1;i>=0;--i)//��Сä��Ϣ
 {
	if(g_game.AllPlayers[i].isContinue==true)//���������û������
	{
		++Index;
		g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrPot=atoi(msg[++Index].c_str());
		if(msg[++Index]!="blind")
			return false;
		//g_game.AllPlayers[i].CurrAction=atoi(msg[++Index].c_str());
		//g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//��¼��ҵ�ÿ�ξ��߽��
		//if(g_game.AllPlayers[i].CurrAction==Fold)//�������
			//g_game.AllPlayers[i].isContinue=false;
	
	}
 }
 for(int i=7;i>=g_Seat;--i) //ׯ����ʱ�뵽��
 {
	 if(g_game.AllPlayers[i].isContinue==true)
	{
		++Index;
		g_game.AllPlayers[i].jetton=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].ReMoney=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrPot=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].CurrAction=atoi(msg[++Index].c_str());
		g_game.AllPlayers[i].actions.push_back(g_game.AllPlayers[i].CurrAction);//��¼��ҵ�ÿ�ξ��߽��
		if(g_game.AllPlayers[i].CurrAction==Fold)//�������
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

//̯����Ϣ
bool Set_ShowDownMsg(const VecStr& msg);
//�ʳط�����Ϣ
bool Set_PotWinMsg(const VecStr& msg)
{
 if(msg.size()!=4 || msg[0]!="pot-win/" || msg[3]!="/pot-win")
			return false;
 int seat=GetSeatNum(atoi(msg[1].c_str()));
 g_game.AllPlayers[seat].jetton+=atoi(msg[2].c_str());//����Ӯ�ҵĳ���
 return true;
}
//��Ϸ������Ϣ
bool Set_GameOver(const VecStr& msg)
{
	 if(msg.size()!=2 || msg[0]!="game-over" )
			return false;
	 g_game.GameOver=true;
	 return true;
 
}
//�������ע����Ϣ
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
//ս��Ares������Ϣ   :player->server.   
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

//��Ϣ����ӿں���(server -> player)
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