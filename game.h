#ifndef _GAME_
#define _GAME_

#include <algorithm>
#include "Player.h"

using namespace std;
/**************************************************
ȫ�ֱ���
**************************************************/

//�����ʶ
extern bool g_invalid;

//extern class Card;
//extern class CT_Value;

//extern class CT_Value;

//����ֵ����
// CT_Value g_CTvalues[7];

//������
//class Card  g_cards[7];


//��ŵ�ǰ�����ƿ���ɵ����ͣ�ÿ�����Ͷ�Ӧ�����һλ��1��ʾ����ɸ����ͣ�����Ϊ����
//ֱ����������Ϊ�±�
extern int isType[9];


/***********************************************************************************
***********************************************************************************/
//����ıȽϺ�����
//����������
int comp_point(Card a,Card b);

//����ɫ���򣨺� �� ÷ ����
int comp_color(Card a, Card b);

//�����Ȱ���ɫ����ͬ���ڲ�����������    pendΪ��β
void CPSort(Card* pbeg,Card* pend);



/*�жϸ����� ������ ���������Ƿ������ĳ�����ͣ�����������isType�����ӦλΪ1��
�����ؾ������͵��ƣ������Ƕ�Ӧ����ֵʱ����ʼ��Ϊpoint������*/

//�Ƿ�Ϊͬ��˳  
Card isStraightFlush(Card* cards,int beg,int end);

//�Ƿ�Ϊ����
Card isFourOne(Card* cards,int beg,int end);

//�Ƿ�Ϊ������
Card isThreeTwo(Card* cards,int beg,int end);


//�Ƿ�Ϊͬ��  �����5�����Ѿ�����ɫ����
Card isFlush(Card* cards,int beg,int end);

//�Ƿ�Ϊ˳��
Card  isStraight(Card* cards,int beg,int end);

//�Ƿ�Ϊ����
Card isThreeCard(Card* cards,int beg ,int end);
	
//�Ƿ�Ϊ����
Card isTwoPair(Card* cards,int beg,int end);
	
	//�Ƿ�Ϊһ��
Card isOnePair(Card* cards,int beg,int end);
	
//�Ƿ�Ϊ����
Card isHighCard(Card*cards,int beg,int end);
	
	
	
/*��ǰ����(���)���ɵ��� �� ���й����Ƶĵó�����5 ��6 ��7 ����*/
CT_Value GetCT_Value(Card* cards,int num);


/*******************************************************************************
���� �Ķ����� ���ʱ�        �Ȳ��ã�������
*************************************************************************************/
//����Ȧ����
extern double probFlop[50];
//ת��Ȧ����
extern double probTurn[47];
//����Ȧ����
extern double probRive[46];

//���ʱ��ʼ������
void probInition();
//��Ȧ���и��ʲ�ѯ����  turn��������Ȧ���� helpnum:�ɰ����ճ��������͵�ʣ��������
double probGet(int turn,int helpnum);




#endif










