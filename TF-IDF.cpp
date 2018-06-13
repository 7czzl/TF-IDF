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
		int pos = word.find(' ');
		word = word.substr(0, pos);
		ignore_words[word] = 1;
	}
	cout << "Loading ignore_words complete!" << endl << endl;
}

vector<map<string, int>> wordTF;//每篇文档中单词的词频
vector<set<string>> words_for_each_article;//每篇文档的所有不同单词
vector<int> word_Quantity_each_article;//每篇文档的单词总数
map<string, int> DF;//包含单词w的文档数量
bool LoadRawData()
{
	//ofstream out("tok-data.txt", ios::trunc);
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
		tokenizer<> tok(Article);//分词
		map<string, int> TF;//词频
		set<string> word_kind;//所有不同单词
		tokenizer<>::iterator beg = tok.begin();
		beg++;
		int i_word = 0;//文章的单词数量
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
				;
			else
				out << word << " ";*/

			i_word++;
			int set_size = word_kind.size();
			word_kind.insert(word);
			TF[word]++;
			if (word_kind.size() > set_size)
				DF[word]++;
		}
		//out << endl;
		word_Quantity_each_article.push_back(i_word);
		wordTF.push_back(TF);
		words_for_each_article.push_back(word_kind);

		TF.clear();
		word_kind.clear();
		if(++i_Article % 1000 == 0)
			cout << i_Article << endl;
	}
	cout << "Loading raw_data Finish!" << endl << endl;
	in.close();
	//out.close();
	return true;
}
//加载Raw-Data,计算TF, DF等值


vector<map<string, double>> wordTF_IDF_each_article;//每个文档所有单词的TF_IDF值
void calculateTF_IDF()
{
	for (int i = 0; i < words_for_each_article.size(); i++)
	{
		map<string, double> wordTF_IDF;
		for (set<string>::iterator iter = words_for_each_article[i].begin(); iter != words_for_each_article[i].end(); iter++)
		{
			double TF = (wordTF[i][*iter] * 1.0) / word_Quantity_each_article[i];
			double IDF = log2((wordTF.size() * 1.0) / (1 + DF[*iter]));
			wordTF_IDF[*iter] = TF * IDF;
		}
		wordTF_IDF_each_article.push_back(wordTF_IDF);
		wordTF_IDF.clear();
	}
}
//计算每个文档所以单词的TF_IDF值


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
	string dimension = "_12.txt";
	string file1 = "feature_words" + dimension;
	string file2 = "feature_words_TF-IDF" + dimension;
	ofstream out1(file1, ios::trunc);
	ofstream out2(file2, ios::trunc);
	for (int i = 0; i < wordTF_IDF_each_article.size(); i++)
	{
		int n = 12;//特征向量维数
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
	out1.close();
	out2.close();
}
//构造N维特征向量

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

	CreateFeatureVector();

	system("pause");
	return 0;
}