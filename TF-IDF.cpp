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

map<string, int> ignore_words;
bool LoadIgnoreWords()
{
	ifstream in("ignore-words.txt");
	if (!in)
	{
		cout << "\"ignore-words.txt\" is not exist!" << endl;
		return false;
	}
	string word;
	while (getline(in, word))
	{
		ignore_words[word] = 1;
	}
	cout << "Loading ignore_words complete!" << endl << endl;
}

vector<map<string, int>> wordTF;//ÿƪ�ĵ��е��ʵĴ�Ƶ
vector<set<string>> words_for_each_article;//ÿƪ�ĵ������в�ͬ����
vector<int> word_Quantity_each_article;//ÿƪ�ĵ��ĵ�������
map<string, int> DF;//��������w���ĵ�����
bool LoadRawData()
{
	ifstream in("raw-data.txt");
	if (!in)
	{
		cout << "\"raw-data.txt\" is not exist!" << endl;
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
		int i_word = 0;//���µĵ�������
		int lower_letter = 'A' - 'a';
		for (; beg != tok.end(); beg++)
		{
			bool Flag = true;
			string word = *beg;
			for (int k = 0; k < word.size(); k++)
			{
				if (word[k] >= 'a' && word[k] <= 'z')
					;
				else if (word[k] >= 'A' && word[k] <= 'Z')
				{
					word[k] = word[k] - lower_letter;
				}
				else
				{
					Flag = false;
					break;
				}
			}
			if (Flag == false)
				continue;

			/*if (ignore_words[word] == 1)
				continue;*/

			i_word++;
			int set_size = word_kind.size();
			word_kind.insert(word);
			TF[word]++;
			if (word_kind.size() > set_size)
				DF[word]++;
		}
		word_Quantity_each_article.push_back(i_word);
		wordTF.push_back(TF);
		words_for_each_article.push_back(word_kind);

		TF.clear();
		word_kind.clear();
		if(++i_Article % 1000 == 0)
			cout << i_Article << endl;
	}
	cout << "Loading raw_data Finish!" << endl << endl;
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
	ofstream out1("feature_words.txt", ios::trunc);
	ofstream out2("feature_words_TF-IDF.txt", ios::trunc);
	for (int i = 0; i < wordTF_IDF_each_article.size(); i++)
	{
		int n = 5;//��������ά��
		vector<pair<string, double>> TF_IDF;
		sortMapByValue(wordTF_IDF_each_article[i], TF_IDF);
		if (n > TF_IDF.size())
			n = TF_IDF.size();
		for (int j = 0; j < n; j++)
		{
			if (ignore_words[TF_IDF[j].first] == 1)
			{
				if (n == TF_IDF.size())
					continue;
				else
				{
					n++;
					continue;
				}
			}
			out1 << TF_IDF[j].first << " ";
			out2 << TF_IDF[j].second << " ";
			//cout << TF_IDF[j].first << "(" << TF_IDF[j].second << ")  ";
		}
		out1 << endl;
		out2 << endl;
		if((i+1) % 1000 == 0)
			cout << i+1 << endl;
		TF_IDF.clear();
	}
	cout << "Creating TF-IDF finish!" << endl << endl;
}
//����Nά��������

int main()
{
	if (!LoadIgnoreWords())
	{
		system("pause");
		return 0;
	}

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