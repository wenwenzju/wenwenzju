#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

void loadHtml(const string& file_name, vector<string>& lines)
{
	ifstream ifs(file_name);
	while(!ifs.eof())
	{
		string line;
		getline(ifs, line);
		lines.push_back(line);
	}
	ifs.close();
}

void writeHtml(const string& file_name, vector<string>& lines)
{
	ofstream ofs(file_name);
	for (int i = 0; i < lines.size(); ++i)
	{
		ofs << lines[i] << endl;
	}
	ofs.close();
}

void loadConfig(const string& file_name, map<string, vector<string> >& config)
{
	ifstream ifs(file_name);
	while(!ifs.eof())
	{
		string line;
		getline(ifs, line);
		if (line.empty())
			continue;
		auto com = line.find_first_of(':');
		string key = line.substr(0, com);
		auto bracket_left = key.find_first_of('[');
		auto bracket_right = key.find_last_of(']');
		string data_desp;
		if (bracket_left != string::npos)
			data_desp = key.substr(bracket_left+1, bracket_right-bracket_left-1);
		auto body_start = com+1;
		while (line[body_start] == ' ') body_start++;
		string body = line.substr(body_start);
		if (data_desp == "article_body")
		{
			while (!ifs.eof())
			{
				body += '\n';
				string line;
				getline(ifs, line);
				body += line;
			}
		}
		else
		{
			string new_body;
			size_t pre = 0;
			auto pattern = body.find("${", pre);
			while (pattern != string::npos)
			{
				new_body += body.substr(pre, pattern-pre);
				auto pattern_right = body.find('}', pattern);
				string pattern_str = body.substr(pattern+2, pattern_right-pattern-2);
				if (config.count(pattern_str))
					new_body += config[pattern_str][0];
				pre = pattern_right+1;
				pattern = body.find("${", pre);
			}
			new_body += body.substr(pre);
			swap(body, new_body);
		}
		config[key.substr(0, bracket_left)].push_back(body);
	}
}

bool allNum(const string& str)
{
	for (auto i = 0; i < str.size(); ++i)
	{
		if (str[i]<'0' || str[i]>'9')
			return false;
	}
	return true;
}

void timelineFactory()
{
	string common_file = "../../timeline/common/index.html";
	string config_file = "config/timeline.txt";
	map<string, vector<string> > config;
	vector<string> index;
	loadHtml(common_file, index);
	loadConfig(config_file, config);

	for (auto iter = config.begin(); iter != config.end(); ++iter)
	{
		if (allNum(iter->first))
		{
			auto line_num = stoi(iter->first);
			string& line = index[line_num-1];
			string new_line;
			size_t pre = 0;
			auto pattern = line.find("???", pre);
			int cnt = 0;
			while (pattern != string::npos && cnt < iter->second.size())
			{
				new_line += line.substr(pre, pattern-pre);
				auto pattern_right = pattern+2;
				new_line += iter->second[cnt];
				pre = pattern_right+1;
				pattern = line.find("???", pre);
				cnt++;
			}
			new_line += line.substr(pre);
			swap(line, new_line);
		}
	}
	writeHtml("index.html", index);
}

int main()
{
	timelineFactory();

	return 0;
}