#include "game.h"


/**************************************************
全局变量
**************************************************/
//错误标识
 bool g_invalid;
//存放当前已有牌可组成的牌型，每种牌型对应数组的一位，1表示能组成该牌型，否则为不能
//直接以牌型作为下标
 int isType[9];

 //翻牌圈概率
double probFlop[50];
//转牌圈概率
 double probTurn[47];
//河牌圈概率
 double probRive[46];



//存放当前已有牌可组成的牌型，每种牌型对应数组的一位，1表示能组成该牌型，否则为不能
//直接以牌型作为下标
//extern int isType[9];


/***********************************************************************************
***********************************************************************************/
//排序的比较函数：
//按点数排序
int comp_point(Card a,Card b)
{
	return a.point < b.point;
	
	
}

//按花色排序（黑 红 梅 方）
int comp_color(Card a, Card b)
{
	return a.color < b.color;
	
}

//排序：先按花色排序，同花内部按点数排序    pend为超尾
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



/*判断给定的 已排序 的五张牌是否能组成某种牌型，若是则设置isType数组对应位为1，
并返回决定牌型的牌，当不是对应类型值时返回始终为point最大的牌*/

//是否为同花顺  
Card isStraightFlush(Card* cards,int beg,int end)
{
	//按点数排序
	//sort(cards+beg,cards+end+1,comp_point);//sort的区间为左闭右开
	bool is=true;
	for(int i=beg;i<end;++i)
	{
		//H4D4H5D5H6D6D7D8  判断出错
		if((cards[i].color != cards[i+1].color)||(cards[i].point+1 != cards[i+1].point))
		{
			is=false;
			break;
		}
	}
	
	if(is)//为同花顺
	{
		isType[ctStraightFlush]=1;
	}
	else
		isType[ctStraightFlush]=0;
	
	return cards[end]; 
}

//是否为四条
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

//是否为三带二
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


//是否为同花  传入的5张牌已经按花色排序
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

//是否为顺子
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

//是否为三条
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
	
//是否为二对
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
	
	//是否为一对
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
	
//是否为高牌
Card isHighCard(Card*cards,int beg,int end)
{
	isType[ctHighCard]=1;
	return cards[end];
}
	
	
	
/*当前牌型(最高)（由底牌 和 已有公共牌的得出）有5 或6 或7 张牌*/
CT_Value GetCT_Value(Card* cards,int num)
{
	Card card;
	CT_Value ctv;
		
	for(int i=0;i<9;++i)
		isType[i]=0;

//是否为同花顺   
	CPSort(cards,cards+num);
	for(int i=num;i>=5;--i)
	{
		card= isStraightFlush(cards,i-5,i-1);
		if(isType[ctStraightFlush]==1)
		{
			ctv.value=card; //牌值
			ctv.type=ctStraightFlush;//牌型
			return ctv;
		}
	}
	//是否为同花
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
	//是否为四条
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

	//是否为三代二
	card=isThreeTwo(cards,0,num-1);
	if(isType[ctThreeTwo]==1)
	{
		ctv.value=card;
		ctv.type=ctThreeTwo;
		return ctv;
	}

	
	//是否为顺子
	card=isStraight(cards,0,num-1);
	if(isType[ctStraight]==1)
	{
		ctv.value=card;
		ctv.type=ctStraight;
		return ctv;
	}
		
	//是否为三条
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
		
	//是否为两对
	card=isTwoPair(cards,0,num-1);
	if(isType[ctTwoPair]==1)
	{
		ctv.value=card;
		ctv.type=ctTwoPair;
		return ctv;
	}
	//是否为一对
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
	//是否为高牌
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
	//正常 不会运行到这里
	return ctv;
}















