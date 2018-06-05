#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost\tokenizer.hpp>
using namespace std;
using namespace boost;


//typedef map<string, int> word_Map;
//typedef word_Map::iterator iter_for_words;

vector<map<string, int>> wordTF;//ÿƪ�ĵ��е��ʵĴ�Ƶ
vector<set<string>> words_for_each_article;//ÿƪ�ĵ������в�ͬ����
vector<int> word_Quantity_each_article;//ÿƪ�ĵ��ĵ�������
map<string, int> DF;//��������w���ĵ�����
bool LoadRawData()
{
	ifstream in("raw-data.txt");
	if (!in)
	{
		cout << "file is not exist!" << endl;
		return false;
	}

	string Article;
	int i_Article = 0;
	//getline(in, Article);
	while (getline(in, Article))
	{
		tokenizer<> tok(Article);//�ִ�
		map<string, int> TF;//��Ƶ
		set<string> word_kind;//���в�ͬ����
		tokenizer<>::iterator beg = tok.begin();
		beg++;
		int i_word = 0;
		for (; beg != tok.end(); beg++)
		{
			bool Flag = true;
			for (int k = 0; k < (*beg).size(); k++)
			{
				if (((*beg)[k] >= 'a' && (*beg)[k] <= 'z') || ((*beg)[k] >= 'A' && (*beg)[k] <= 'Z'))
					;
				else
				{
					Flag = false;
					break;
				}
			}
			if (Flag == false)
				continue;

			i_word++;
			int set_size = word_kind.size();
			word_kind.insert(*beg);
			TF[*beg]++;
			if (word_kind.size() > set_size)
				DF[*beg]++;
		}
		word_Quantity_each_article.push_back(i_word);
		wordTF.push_back(TF);
		words_for_each_article.push_back(word_kind);

		TF.clear();
		word_kind.clear();
		cout << ++i_Article << endl;
	}
	cout << "Finish!" << endl;
	return true;
}
//����Raw-Data,����TF, DF��ֵ


vector<map<string, double>> wordTF_IDF_each_article;//ÿ���ĵ����е��ʵ�TF_IDFֵ
void calculateTF_IDF()
{
	for (int i = 0; i < words_for_each_article.size(); i++)
	{
		map<string, double> wordTF_IDF;
		for (set<string>::iterator iter = words_for_each_article[i].begin(); iter != words_for_each_article[i].end(); iter++)
		{
			double TF = (wordTF[i][*iter] * 1.0) / word_Quantity_each_article[i];
			double IDF = log10((wordTF.size() * 1.0) / (1 + DF[*iter]));
			wordTF_IDF[*iter] = TF * IDF;
		}
		wordTF_IDF_each_article.push_back(wordTF_IDF);
		wordTF_IDF.clear();
	}
}
//����ÿ���ĵ����Ե��ʵ�TF_IDFֵ


bool cmp(const pair<string, double> &x, const pair<string, double> &y)
{
	return x.second > y.second;
}

void sortMapByValue(map<string, double> tMap, vector<pair<string, double>> &tVector)
{
	for (map<string, double>::iterator iter = tMap.begin(); iter != tMap.end(); iter++)
	{
		tVector.push_back(make_pair(iter->first, iter->second));
	}
	sort(tVector.begin(), tVector.end(), cmp);
}

void CreateFeatureVector()
{
	for (int i = 0; i < wordTF_IDF_each_article.size(); i++)
	{
		int n = 5;//��������ά��
		vector<pair<string, double>> TF_IDF;
		sortMapByValue(wordTF_IDF_each_article[i], TF_IDF);
		if (n > TF_IDF.size())
			n = TF_IDF.size();
		for (int j = 0; j < n; j++)
		{
			cout << TF_IDF[j].first << "(" << TF_IDF[j].second << ")  ";
		}
		cout << i << endl << endl;
		TF_IDF.clear();
	}
}
//����Nά��������

int main()
{
	if (!LoadRawData())
	{
		system("pause");
		return 0;
	}
	calculateTF_IDF();
	/*for (int i = 0; i < wordTF_IDF_each_article.size(); i++)
	{
		if (wordTF_IDF_each_article[i].size() < 5)
			cout << i << ", " << wordTF_IDF_each_article[i].size() << endl;
	}*/
	CreateFeatureVector();

	system("pause");
	return 0;
}