#include "game.h"


/**************************************************
ȫ�ֱ���
**************************************************/
//�����ʶ
 bool g_invalid;
//��ŵ�ǰ�����ƿ���ɵ����ͣ�ÿ�����Ͷ�Ӧ�����һλ��1��ʾ����ɸ����ͣ�����Ϊ����
//ֱ����������Ϊ�±�
 int isType[9];

 //����Ȧ����
double probFlop[50];
//ת��Ȧ����
 double probTurn[47];
//����Ȧ����
 double probRive[46];



//��ŵ�ǰ�����ƿ���ɵ����ͣ�ÿ�����Ͷ�Ӧ�����һλ��1��ʾ����ɸ����ͣ�����Ϊ����
//ֱ����������Ϊ�±�
//extern int isType[9];


/***********************************************************************************
***********************************************************************************/
//����ıȽϺ�����
//����������
int comp_point(Card a,Card b)
{
	return a.point < b.point;
	
	
}

//����ɫ���򣨺� �� ÷ ����
int comp_color(Card a, Card b)
{
	return a.color < b.color;
	
}

//�����Ȱ���ɫ����ͬ���ڲ�����������    pendΪ��β
void CPSort(Card* pbeg,Card* pend)
{
	g_invalid=false;
	if(pbeg==NULL)
	{
		g_invalid=true;
		return;
	}

	sort(pbeg,pend,comp_color);
	Card* temp1=pbeg;
	Card* temp2=pbeg;
	for(;temp1<pend && temp2<pend;)
	{
		while((temp2+1)->color == temp1->color)
		{
		 ++temp2;
		}
		sort(temp1,temp2+1,comp_point);
		temp1=temp2+1;
		++temp2;	
	}
}



/*�жϸ����� ������ ���������Ƿ������ĳ�����ͣ�����������isType�����ӦλΪ1��
�����ؾ������͵��ƣ������Ƕ�Ӧ����ֵʱ����ʼ��Ϊpoint������*/

//�Ƿ�Ϊͬ��˳  
Card isStraightFlush(Card* cards,int beg,int end)
{
	//����������
	//sort(cards+beg,cards+end+1,comp_point);//sort������Ϊ����ҿ�
	bool is=true;
	for(int i=beg;i<end;++i)
	{
		//H4D4H5D5H6D6D7D8  �жϳ���
		if((cards[i].color != cards[i+1].color)||(cards[i].point+1 != cards[i+1].point))
		{
			is=false;
			break;
		}
	}
	
	if(is)//Ϊͬ��˳
	{
		isType[ctStraightFlush]=1;
	}
	else
		isType[ctStraightFlush]=0;
	
	return cards[end]; 
}

//�Ƿ�Ϊ����
Card isFourOne(Card* cards,int beg,int end)
{
	if(cards[beg+1].point==cards[end].point)
	{
		isType[ctFourOne]=1;
			return cards[end];
	}
	if(cards[beg].point==cards[end-1].point)
	{
		isType[ctFourOne]=1;
		return cards[end-1];
	}
	
	isType[ctFourOne]=0;
	return cards[end];
}

//�Ƿ�Ϊ������
Card isThreeTwo(Card* cards,int beg,int end)
{
	if(end-beg >=4)
	{
		for (;beg<3;beg++)
		{
			if (cards[beg].point == cards[beg+1].point && cards[beg].point == cards[beg+2].point)
			{
				int i= beg+3;
				for (;i<end;i++)
				{
					if (cards[i].point == cards[i+1].point)
					{
							isType[ctThreeTwo]=1;
							return cards[beg];

					}
				}
			} 
			else if (cards[beg].point == cards[beg+1].point && cards[beg].point != cards[beg+2].point)
			{
				int i = beg+2;
				for (;i<end-1;i++)
				{
					if (cards[i].point == cards[i+1].point && cards[i].point == cards[i+2].point)
					{
						isType[ctThreeTwo]=1;
						return cards[beg];
					}
				}
			}						
		}		
	}
	isType[ctThreeTwo]=0;
	return cards[end];	
}


//�Ƿ�Ϊͬ��  �����5�����Ѿ�����ɫ����
Card isFlush(Card* cards,int beg,int end)
{
	for(int i=beg;i<end;++i)
	{
		if(cards[i].color!=cards[i+1].color)
		{
			isType[ctFlush]=0;
			return cards[end];
		}
	}
	isType[ctFlush]=1;
	return cards[end];
}

//�Ƿ�Ϊ˳��
Card  isStraight(Card* cards,int beg,int end)
{
	int Dvalue=0;
	int p=end;
	for(;beg!=end;++beg)
	{
		if(cards[beg].point+1 == cards[beg+1].point)
		{
			Dvalue++;
			p=beg+1;
		}
		else  if(cards[beg].point < cards[beg+1].point-1  )
		{
			p=beg+1;
			Dvalue=0;
		}
	} 
	if (Dvalue>=4)
	{
		isType[ctStraight]=1;
		return cards[p];
	}
	isType[ctStraight]=0;
	return cards[end];	
}

//�Ƿ�Ϊ����
Card isThreeCard(Card* cards,int beg ,int end)
{
	if(cards[beg].point==cards[beg+2].point)
	{
		isType[ctThreeCard]=1;
		return cards[beg];
	}
	if(cards[beg+2].point==cards[end].point)
	{
		isType[ctThreeCard]=1;
		return cards[end];
	}
	isType[ctThreeCard]=0;
	return cards[end];
}
	
//�Ƿ�Ϊ����
Card isTwoPair(Card* cards,int beg,int end)
{
	isType[ctTwoPair]=0;
	int p=end;
	if(end-beg>=4)
	{
		int pair=0;
		for (;beg<end;beg++)
		{
			if (cards[beg].point == cards[beg+1].point)
			{
				pair++;
				beg+=1;
				p=beg;
			}
		}
		if (pair>1)
		{
			isType[ctTwoPair]=1;
			return cards[p];
		}
	}
	isType[ctTwoPair]=0; 
	return cards[end];
}
	
	//�Ƿ�Ϊһ��
Card isOnePair(Card* cards,int beg,int end)
{
		if(cards[beg].point==cards[beg+1].point)
		{
			isType[ctOnePair]=1;
			return cards[beg+1];
		}
		if(cards[beg+1].point==cards[beg+2].point)
		{
			isType[ctOnePair]=1;
			return cards[beg+2];
		}
		if(cards[beg+2].point==cards[beg+3].point)
		{
			isType[ctOnePair]=1;
			return cards[beg+3];
		}
		if(cards[beg+3].point==cards[end].point)
		{
			isType[ctOnePair]=1;
			return cards[end];
		}
		
		isType[ctOnePair]=0;
		return cards[end];
	}
	
//�Ƿ�Ϊ����
Card isHighCard(Card*cards,int beg,int end)
{
	isType[ctHighCard]=1;
	return cards[end];
}
	
	
	
/*��ǰ����(���)���ɵ��� �� ���й����Ƶĵó�����5 ��6 ��7 ����*/
CT_Value GetCT_Value(Card* cards,int num)
{
	Card card;
	CT_Value ctv;
		
	for(int i=0;i<9;++i)
		isType[i]=0;

//�Ƿ�Ϊͬ��˳   
	CPSort(cards,cards+num);
	for(int i=num;i>=5;--i)
	{
		card= isStraightFlush(cards,i-5,i-1);
		if(isType[ctStraightFlush]==1)
		{
			ctv.value=card; //��ֵ
			ctv.type=ctStraightFlush;//����
			return ctv;
		}
	}
	//�Ƿ�Ϊͬ��
	for(int i=num;i>=5;--i)
	{
		card=isFlush(cards,i-5,i-1);
		if(isType[ctFlush]==1)
		{
			ctv.value=card;
			ctv.type=ctFlush;
			return ctv;
		}
	}
	//�Ƿ�Ϊ����
	sort(cards,cards+num,comp_point);
	for(int i=num;i>=5;--i)
	{
		card=isFourOne(cards,i-5,i-1);
		if(isType[ctFourOne]==1)
		{
			ctv.value=card;
			ctv.type=ctFourOne;
			return ctv;
		}
	}

	//�Ƿ�Ϊ������
	card=isThreeTwo(cards,0,num-1);
	if(isType[ctThreeTwo]==1)
	{
		ctv.value=card;
		ctv.type=ctThreeTwo;
		return ctv;
	}

	
	//�Ƿ�Ϊ˳��
	card=isStraight(cards,0,num-1);
	if(isType[ctStraight]==1)
	{
		ctv.value=card;
		ctv.type=ctStraight;
		return ctv;
	}
		
	//�Ƿ�Ϊ����
	for(int i=num;i>=5;--i)
	{
		card=isThreeCard(cards,i-5,i-1);
		if(isType[ctThreeCard]==1)
		{
			ctv.value=card;
			ctv.type=ctThreeCard;
			return ctv;
		}
	}
		
	//�Ƿ�Ϊ����
	card=isTwoPair(cards,0,num-1);
	if(isType[ctTwoPair]==1)
	{
		ctv.value=card;
		ctv.type=ctTwoPair;
		return ctv;
	}
	//�Ƿ�Ϊһ��
	for(int i=num;i>=5;--i)
	{
		card=isOnePair(cards,i-5,i-1);
		if(isType[ctOnePair]==1)
		{
			ctv.value=card;
			ctv.type=ctOnePair;
			return ctv;
		}
	}
	//�Ƿ�Ϊ����
	for(int i=num;i>=5;--i)
	{
		card=isHighCard(cards,i-5,i-1);
		if(isType[ctHighCard]==1)
		{
			ctv.value=card;
			ctv.type=ctHighCard;
			return ctv;
		}
	}
	//���� �������е�����
	return ctv;
}















