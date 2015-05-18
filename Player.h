#ifndef _PLAYER_
#define _PLAYER_

#include <vector>
#include <string>
using namespace std;

typedef vector<string> VecStr;

//���������Ϣ
extern char* msgbuff;
//��Ž�������Ϣ��ֵĽ��
extern VecStr g_MsgVec;
//��ʼ�����
extern const  int g_PlayerNum;
//�ƾ�
extern class Game g_game;

/*****************************************************
����ս��Ares
********************************************************/
//ս���pid
extern int g_Pid;
//ս���name
extern char* g_name;
//ս�����λ��
extern int g_Seat;   
//��ʼ����
extern const int  g_jetton;
//��ʼ���
extern const int g_Money;
//ս�� 
extern class Player& Ares; 


//��ɫ
enum Color{
	NoColor=0,Spade=1,Heart,Club,Diamond  //�� �� ÷ ��   NOColor��ʾ��ɫδ֪
};

//����    p0ΪĬ�ϵ�����������δ֪
enum Point{
	p0=0,p2=2,p3,p4,p5,p6,p7,p8,p9,p10,pJ,pQ,pK,pA
};

//һ����
class Card{
public:
	Card():color(NoColor),point(p0){ }
public:
	int color;//��ɫ
	int point;//����
};

//����
enum Action{
	NoAction=0,Check=1,Call,Raise,AllIn,Fold //���� ��ע ��ע ȫѺ ���ƣ�NoAction��ʾ��δ����״̬
};


//����
enum CardType{
	ctHighCard=0,  //���� 0
		ctOnePair,      //һ��1 
		ctTwoPair,	//���� 2 
		ctThreeCard,    //���� 3 
		ctStraight,	//˳�� 4
		ctFlush,	//ͬ��  5
		ctThreeTwo,	//������  ��« 6
		ctFourOne,	//�Ĵ�һ  ���� 7
		ctStraightFlush //ͬ��˳ 8
};

//���ͺ���ֵ����K��ͬ��˳)
class CT_Value{
public:
	CT_Value():type(ctHighCard),value(){ }
public:
	CardType type;
	Card  value;
};

//����  ת��  ����
enum GameTurn{
	PreHold=0,Hold=1,Flop,Turn,Rive   //PreHold��ʾ�ƾֻ�δ��ʼ
};

/*��ǰ����(���)���ɵ��� �� ���й����Ƶĵó�����5 ��6 ��7 ����*/
extern CT_Value GetCT_Value(Card* cards,int num);

//��������ȫ�ֺ���
Card* GetPubCards();
int GetCardsNum();


//����
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
	}						//��ȡ��ǰ�������	(5 6 7����)
	int GetSeat(){return seat;}					//��ȡ��Ϊ��Ϣ
	int GetTotalPot(){return TotalPot;}			//��ȡ�����ۼ���Ͷע��
	int Getjetton(){return jetton;}				//��ȡ��ǰ�ʳ��ܶ�
	int GetRemony(){return ReMoney;}			//��ȡʣ��������
	Card* GetHoldCs(){return HoldCards;}		//��ȡ����
	vector<int> GetActions(){return actions;}	//��ȡ��ʷaction��¼
	int GetCurrAction(){return CurrAction;}		//��ȡ���һ��action
	bool GetisContinue(){return isContinue;}	//��ȡ�ƾ��Ƿ��ڼ���

	//���Ժ���1
	//���Ժ���2
	//���Ժ���3

public:
	int pid;			//��ұ�ʶ
	char* name;			
	int seat;			//��λ��ţ���ׯ�ҵ����λ�ã�
	int CurrPot;		//��ǰ��Ҫ����ע��
	int TotalPot;		//�����ۼ���Ͷע��
	int jetton;			//���г���
	int ReMoney;		//ʣ��������

	Card HoldCards[2];	//����
	vector<int> actions;//�����ѷ�����ÿһ�ε��ж�����
	int CurrAction;		//�������ǰ��һ��action
	bool isContinue;    //�������Ƿ����˳�����,������˳������ٸ������Ա��Ϣ
	CT_Value ctv;		//��ǰ�������
};

//�ƾ�
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
	Player* AllPlayers;  //�������
	static Card* PubCard;		//������
	int players;		//��ǰ�������
	int TotalMoney;		//��ǰ�׳�����
	int turn;			//��ǰ�ƾֽ�չ���緭��ǰ  ����Ȧ ת��Ȧ ����Ȧ��
	int RaisePlayer;    //��ǰȦ��ע����
	static int CardsNum;		//����+�ѷ�����������
	bool GameOver;		//��Ϸ������־
	int BlindsNum;          //Сäע���
};




/**********************************************
ȫ�� ��Ϣ���ú���
*********************************************/

//�ַ�����ֺ�������s��c��֣�ÿ���ֶδ浽lstr_vec
void split(const string& s, char c, VecStr& Vec);
//������Ϣ�������䰴�ֶβ��
void SplitMsg(char* buffer, VecStr& msg);
//������λ��Ϣ
bool Set_SeatMsg(const VecStr& msg);
//���ô�Сäע
bool Set_BlindMsg(const VecStr& msg);
//��������
bool Set_HoldCsMsg(const VecStr& msg);
//����ѯ����Ϣ
bool Set_InquireMsg(const VecStr& msg);
//���÷���
bool Set_FlopMsg(const VecStr& msg);
//����ת��
bool Set_TurnMsg(const VecStr& gmsg);
//���ú���
bool Set_RiveMsg(const VecStr& msg);
//����̯�ƽ��
bool Set_ShowDownMsg(const VecStr& msg);
//���òʳط�����
bool Set_PotWinMsg(const VecStr& msg);
//����gameover��־
bool Set_GameOver(const VecStr& msg);
//����action��Ϣ��player->server
string Set_ActionMsg(int action,int raisenum=0);
//����ע����Ϣ��payer->server
string Set_RegMsg(int id,char* Name);

//��Ϣ������ýӿں���
bool Set_Msg(const char *buf );


#endif