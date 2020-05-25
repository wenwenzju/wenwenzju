#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <locale>
#include <codecvt>

using namespace std;

std::string UnicodeToUTF8(const std::wstring & wstr)
{
	std::string ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(wstr);
	} catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

std::wstring UTF8ToUnicode(const std::string & str)
{
	std::wstring ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(str);
	} catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

std::string UnicodeToANSI(const std::wstring & wstr)
{
	setlocale(LC_CTYPE, "");
	std::string ret;
	std::mbstate_t state = 0;
	const wchar_t *src = wstr.data();
	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< char [] > buff(new char[len + 1]);
		len = std::wcsrtombs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}

std::wstring ANSIToUnicode(const std::string & str)
{
	std::wstring ret;
	std::mbstate_t state;
	const char *src = str.data();
	size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< wchar_t [] > buff(new wchar_t[len + 1]);
		len = std::mbsrtowcs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}

std::string UTF8ToANSI(const std::string & str)
{
	return UnicodeToANSI(UTF8ToUnicode(str));
}

std::string ANSIToUTF8(const std::string & str)
{
	return UnicodeToUTF8(ANSIToUnicode(str));
}

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

void tagsFactory()
{
	string config_file_path = "config/tags.txt";
	string index_file_path = "../../tags/index.html";
	ifstream config_file(config_file_path);
	string line;
	getline(config_file, line);
	string ymd = line.substr(line.find(':')+2);
	getline(config_file, line);
	string hms = line.substr(line.find(':')+2);
	getline(config_file, line);
	string category = UTF8ToANSI(line.substr(line.find(':')+2));
	getline(config_file, line);
	int cnt = stoi(line.substr(line.find(':')+2));

	vector<string> index;
	loadHtml(index_file_path, index);
	string& line271 = index[271];

	string new_line;
	size_t pre = 0;
	auto pattern = line271.find('"', pre);
	new_line += line271.substr(pre, pattern-pre+1);
	new_line += ymd;
	new_line += "T";
	new_line += hms;
	new_line += "Z";
	pre = line271.find('"', pattern+1);
	pattern = line271.find('>', pre+1);
	new_line += line271.substr(pre, pattern-pre+1);
	new_line += ymd;
	pattern = line271.find('<', pattern+1);
	new_line += line271.substr(pattern);
	swap (new_line, line271);

	string* line_category = nullptr;
	if (category == "深度学习")
	{
		line_category = &index[325];
	}
	else if (category == "机器学习")
	{
		line_category = &index[326];
	}
	else if (category == "算法")
	{
		line_category = &index[327];
	}
	else
	{
		line_category = &index[328];
	}
	{
		string pattern = "class=\"category-list-count\">";
		auto i1 = line_category->find(pattern)+pattern.size();
		string new_line;
		new_line += line_category->substr(0, i1);
		auto i2 = line_category->find('<', i1);
		auto tot = stoi(line_category->substr(i1, i2-i1));
		tot += cnt;
		new_line += to_string((long long)tot);
		new_line += line_category->substr(i2);
		swap(new_line, *line_category);
	}

	writeHtml("tags_index.html", index);
}


class CategoryArchive
{
public:
	int year;
	int month;
	int day;
	string category;
	string name;
	string hms;
	string ymd;
	string md;
	string description;
	void parseYMD()
	{
		if (ymd.empty())
			return;
		size_t start = 0;
		year = stoi(ymd.substr(start, ymd.find('-', start)-start));
		start = ymd.find('-', start)+1;
		md = ymd.substr(start);
		month = stoi(ymd.substr(start, ymd.find('-', start)-start));
		start = ymd.find('-', start)+1;
		day = stoi(ymd.substr(start));
	}

};
void categoryFactory()
{
	string config_file = "config/timeline.txt";
	map<string, vector<string> > config;
	loadConfig(config_file, config);

	string index_file = "../../categories/";
	index_file += config["category"][0];
	index_file += "/index.html";
	index_file = UTF8ToANSI(index_file);
	vector<string> index;
	loadHtml(index_file, index);

	CategoryArchive to_add;
	to_add.ymd = config["ymd"][0];
	to_add.hms = config["hms"][0];
	to_add.name = config["name"][0];
	to_add.category = config["category"][0];
	to_add.description = config["description"][0];
	to_add.parseYMD();

	vector<CategoryArchive> archives;
	archives.push_back(to_add);
	int section_start = -1, section_end = -1;
	for (int i = 0; i < index.size(); ++i)
	{
		if (index[i].find("<section class=\"archives-wrap\">") != string::npos)
		{
			if (section_start < 0)
				section_start = i;
			string& tmp = index[i+2];
			string year_start_str = "class=\"archive-year\">";
			auto year_start_index = tmp.find(year_start_str)+year_start_str.size();
			string section_year = tmp.substr(year_start_index, 4);
			for (int j = 3; i+j < index.size(); ++j)
			{
				if (index[i+j].find("</section>") != string::npos)
				{
					section_end = i+j;
					i = i+j;
					break;
				}
				if (index[i+j].find("<div class=\"archives\">") != string::npos)
				{
					CategoryArchive arch;
					arch.year = stoi(section_year);
					string time_str = index[i+j+6];
					string date_start_str = "<time datetime=\"";
					auto date_start_index = time_str.find(date_start_str)+date_start_str.size();
					arch.ymd = time_str.substr(date_start_index, time_str.find('T', date_start_index)-date_start_index);
					auto time_start_index = time_str.find('T', date_start_index) + 1;
					arch.hms = time_str.substr(time_start_index, time_str.find('Z', time_start_index)-time_start_index);
					arch.parseYMD();

					string detail_str = index[i+j+12];
					string detail_start_str = "<a class=\"article-title\" href=\"";
					auto detail_start_index = detail_str.find(detail_start_str)+detail_start_str.size();
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// timeline
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// 2015
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// 其他
					arch.category = detail_str.substr(detail_start_index, detail_str.find('/', detail_start_index)-detail_start_index);
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// c++调用
					arch.name = detail_str.substr(detail_start_index, detail_str.find('/', detail_start_index)-detail_start_index);
					detail_start_index = detail_str.find('/', detail_start_index)+3;
					arch.description = detail_str.substr(detail_start_index, detail_str.find("</a>", detail_start_index)-detail_start_index);
					archives.push_back(arch);
				}
			}
		}
	}

	sort(archives.begin(), archives.end(), [](CategoryArchive& a, CategoryArchive& b)->bool
	{
		if (a.year > b.year) return true;
		else if (a.year == b.year)
		{
			if (a.month > b.month) return true;
			else if (a.month == b.month) return a.day > b.day;
			else return false;
		}
		else
			return false;
	});

	if (section_start >= 0)
	{
		index.erase(index.begin()+section_start, index.begin()+section_end+1);
	}
	vector<string> new_archives;
	int pre_year = 0;
	for (int i = 0; i < archives.size(); ++i)
	{
		string year_str = to_string((long long)archives[i].year);
		if (pre_year != archives[i].year)
		{
			pre_year = archives[i].year;
			new_archives.emplace_back("      <section class=\"archives-wrap\">");
			new_archives.emplace_back("        <div class=\"archive-year-wrap\">");
			string tmp("          <a href=\"./archives/");
			tmp += year_str + "/\" class=\"archive-year\">" + year_str + "</a>";
			new_archives.emplace_back(tmp);
			new_archives.emplace_back("        </div>");
			new_archives.emplace_back("");
			new_archives.emplace_back("        <div class=\"archives\">");
		}
		new_archives.emplace_back("            <article class=\"archive-article archive-type-post\">");
		new_archives.emplace_back("              <div class=\"archive-article-inner\">");
		new_archives.emplace_back("                <header class=\"archive-article-header\">");
		new_archives.emplace_back("                      <div class=\"article-meta\">");
		new_archives.emplace_back("                        <a href=\"./timeline/"+year_str+"/"+archives[i].category+"/"+archives[i].name+"/\""+" class=\"archive-article-date\">");
		new_archives.emplace_back("                          <time datetime=\""+archives[i].ymd+"T"+archives[i].hms+"Z\" itemprop=\"datePublished\">"+archives[i].md+"</time>");
		new_archives.emplace_back("                        </a>");
		new_archives.emplace_back("                      </div>");
		new_archives.emplace_back("");
		new_archives.emplace_back("");
		new_archives.emplace_back("                    <h1 itemprop=\"name\">");
		new_archives.emplace_back("                      <a class=\"article-title\" href=\"./timeline/"+year_str+"/"+archives[i].category+"/"+archives[i].name+"/\">"+archives[i].description+"</a>");
		new_archives.emplace_back("                    </h1>");
		new_archives.emplace_back("");
		new_archives.emplace_back("");
		new_archives.emplace_back("                    <div class=\"article-info info-on-archive\">");
		new_archives.emplace_back("                        <div class=\"article-category tagcloud\">");
		new_archives.emplace_back("                        <a class=\"article-category-link\" href=\"./categories/"+archives[i].category+"/\">"+archives[i].category+"</a>");
		new_archives.emplace_back("                        </div>");
		new_archives.emplace_back("                    </div>");
		new_archives.emplace_back("                    <div class=\"clearfix\"></div>");
		new_archives.emplace_back("                </header>");
		new_archives.emplace_back("              </div>");
		new_archives.emplace_back("            </article>");
		
		if (i == archives.size()-1 || archives[i].year != archives[i+1].year)
		{
			new_archives.emplace_back("        </div>");
			new_archives.emplace_back("      </section>");
		}
	}
	index.insert(index.begin()+section_start, new_archives.begin(), new_archives.end());
	string to_write = to_add.category+"_"+to_add.name+".html";
	writeHtml(UTF8ToANSI(to_write), index);
}

void categoryFactory()
{
	string config_file = "config/timeline.txt";
	map<string, vector<string> > config;
	loadConfig(config_file, config);

	string index_file = "../../archives/index.html";
	loadHtml(index_file, index);

	CategoryArchive to_add;
	to_add.ymd = config["ymd"][0];
	to_add.hms = config["hms"][0];
	to_add.name = config["name"][0];
	to_add.category = config["category"][0];
	to_add.description = config["description"][0];
	to_add.parseYMD();

	vector<CategoryArchive> archives;
	archives.push_back(to_add);
	int section_start = -1, section_end = -1;
	for (int i = 0; i < index.size(); ++i)
	{
		if (index[i].find("<section class=\"archives-wrap\">") != string::npos)
		{
			if (section_start < 0)
				section_start = i;
			string& tmp = index[i+2];
			string year_start_str = "class=\"archive-year\">";
			auto year_start_index = tmp.find(year_start_str)+year_start_str.size();
			string section_year = tmp.substr(year_start_index, 4);
			for (int j = 3; i+j < index.size(); ++j)
			{
				if (index[i+j].find("</section>") != string::npos)
				{
					section_end = i+j;
					i = i+j;
					break;
				}
				if (index[i+j].find("<div class=\"archives\">") != string::npos)
				{
					CategoryArchive arch;
					arch.year = stoi(section_year);
					string time_str = index[i+j+6];
					string date_start_str = "<time datetime=\"";
					auto date_start_index = time_str.find(date_start_str)+date_start_str.size();
					arch.ymd = time_str.substr(date_start_index, time_str.find('T', date_start_index)-date_start_index);
					auto time_start_index = time_str.find('T', date_start_index) + 1;
					arch.hms = time_str.substr(time_start_index, time_str.find('Z', time_start_index)-time_start_index);
					arch.parseYMD();

					string detail_str = index[i+j+12];
					string detail_start_str = "<a class=\"article-title\" href=\"";
					auto detail_start_index = detail_str.find(detail_start_str)+detail_start_str.size();
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// timeline
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// 2015
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// 其他
					arch.category = detail_str.substr(detail_start_index, detail_str.find('/', detail_start_index)-detail_start_index);
					detail_start_index = detail_str.find('/', detail_start_index)+1;		// c++调用
					arch.name = detail_str.substr(detail_start_index, detail_str.find('/', detail_start_index)-detail_start_index);
					detail_start_index = detail_str.find('/', detail_start_index)+3;
					arch.description = detail_str.substr(detail_start_index, detail_str.find("</a>", detail_start_index)-detail_start_index);
					archives.push_back(arch);
				}
			}
		}
	}

	sort(archives.begin(), archives.end(), [](CategoryArchive& a, CategoryArchive& b)->bool
	{
		if (a.year > b.year) return true;
		else if (a.year == b.year)
		{
			if (a.month > b.month) return true;
			else if (a.month == b.month) return a.day > b.day;
			else return false;
		}
		else
			return false;
	});

	if (section_start >= 0)
	{
		index.erase(index.begin()+section_start, index.begin()+section_end+1);
	}
	vector<string> new_archives;
	int pre_year = 0;
	for (int i = 0; i < archives.size(); ++i)
	{
		string year_str = to_string((long long)archives[i].year);
		if (pre_year != archives[i].year)
		{
			pre_year = archives[i].year;
			new_archives.emplace_back("      <section class=\"archives-wrap\">");
			new_archives.emplace_back("        <div class=\"archive-year-wrap\">");
			string tmp("          <a href=\"./archives/");
			tmp += year_str + "/\" class=\"archive-year\">" + year_str + "</a>";
			new_archives.emplace_back(tmp);
			new_archives.emplace_back("        </div>");
			new_archives.emplace_back("");
			new_archives.emplace_back("        <div class=\"archives\">");
		}
		new_archives.emplace_back("            <article class=\"archive-article archive-type-post\">");
		new_archives.emplace_back("              <div class=\"archive-article-inner\">");
		new_archives.emplace_back("                <header class=\"archive-article-header\">");
		new_archives.emplace_back("                      <div class=\"article-meta\">");
		new_archives.emplace_back("                        <a href=\"./timeline/"+year_str+"/"+archives[i].category+"/"+archives[i].name+"/\""+" class=\"archive-article-date\">");
		new_archives.emplace_back("                          <time datetime=\""+archives[i].ymd+"T"+archives[i].hms+"Z\" itemprop=\"datePublished\">"+archives[i].md+"</time>");
		new_archives.emplace_back("                        </a>");
		new_archives.emplace_back("                      </div>");
		new_archives.emplace_back("");
		new_archives.emplace_back("");
		new_archives.emplace_back("                    <h1 itemprop=\"name\">");
		new_archives.emplace_back("                      <a class=\"article-title\" href=\"./timeline/"+year_str+"/"+archives[i].category+"/"+archives[i].name+"/\">"+archives[i].description+"</a>");
		new_archives.emplace_back("                    </h1>");
		new_archives.emplace_back("");
		new_archives.emplace_back("");
		new_archives.emplace_back("                    <div class=\"article-info info-on-archive\">");
		new_archives.emplace_back("                        <div class=\"article-category tagcloud\">");
		new_archives.emplace_back("                        <a class=\"article-category-link\" href=\"./categories/"+archives[i].category+"/\">"+archives[i].category+"</a>");
		new_archives.emplace_back("                        </div>");
		new_archives.emplace_back("                    </div>");
		new_archives.emplace_back("                    <div class=\"clearfix\"></div>");
		new_archives.emplace_back("                </header>");
		new_archives.emplace_back("              </div>");
		new_archives.emplace_back("            </article>");

		if (i == archives.size()-1 || archives[i].year != archives[i+1].year)
		{
			new_archives.emplace_back("        </div>");
			new_archives.emplace_back("      </section>");
		}
	}
	index.insert(index.begin()+section_start, new_archives.begin(), new_archives.end());
	string to_write = to_add.category+"_"+to_add.name+".html";
	writeHtml(UTF8ToANSI(to_write), index);
}

int main()
{
	//timelineFactory();
	//tagsFactory();
	categoryFactory();

	return 0;
}