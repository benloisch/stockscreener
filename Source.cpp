#include <iostream>
#include <vector>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fstream>
using namespace std;

#include "csv.h"
#include "ImageBMP.h"

class Equity {
public:
	string industry;
	unsigned long long marketCap;
	unsigned int lengthOfDays;
	string ticker;
	vector<string> date;
	vector<float> open;
	vector<float> high;
	vector<float> low;
	vector<float> close;
	vector<unsigned int> volume;

	Equity() {
		industry = "NA";
		marketCap = 0;
	}
};

vector<string> preTicker;
vector<unsigned long long> currentMarketCap;
vector<string> tickerIndustry;

vector<Equity> stocks;
vector<Equity> etfs;
unsigned int stockLoadIndex = 0;
unsigned int etfLoadIndex = 0;

unsigned int loadXEquities = 0;

void loadEquityFolder(string folderPath, string equityType, string specificEquity) {
	string directory = folderPath;

	io::CSVReader<3> inCapAndIndustry("C:\\Users\\benlo\\Documents\\Stock Data\\data\\tickerData\\tickerCapIndustry.csv");
	try {
		inCapAndIndustry.read_header(io::ignore_extra_column, "Ticker", "Market Cap", "Industry");
	}
	catch (...) {
		cout << "Error reading C:\\Users\\benlo\\Documents\\Stock Data\\data\\tickerData\\tickerCapIndustry.csv" << endl;
	}

	std::string tickerName;
	std:string i_marketCap;
	std::string industryString;
	while (inCapAndIndustry.read_row(tickerName, i_marketCap, industryString)) {
		preTicker.push_back(tickerName);
		if (i_marketCap != "") {
			currentMarketCap.push_back(stoull(i_marketCap));
		}
		else {
			currentMarketCap.push_back(0);
		}
		
		tickerIndustry.push_back(industryString);
	}

	_finddata_t info;
	intptr_t hFile;

	folderPath = directory + "*.*";
	const char* fp = folderPath.c_str();

	if ((hFile = _findfirst(fp, &info)) != -1L) {

		_findnext(hFile, &info); //read "." hidden directory file

		while (_findnext(hFile, &info) == 0) {

			if (specificEquity != "") {
				if (info.name != specificEquity) {
					continue;
				}
			}

			//loadXEquities++;
			//if (loadXEquities > 500) {
			//	break;
			//}

			if (equityType == "etf") {
				etfs.push_back(Equity());
			}
			else {
				stocks.push_back(Equity());
			}

			string fileName = info.name;
			string tickerLower = fileName.substr(0, fileName.find('.'));
			string tickerUpper = tickerLower;

			std::transform(tickerUpper.begin(), tickerUpper.end(), tickerUpper.begin(), ::toupper);
			if (equityType == "etf") {
				etfs[etfLoadIndex].ticker = tickerUpper;
			}
			else {
				stocks[stockLoadIndex].ticker = tickerUpper;
			}

			ifstream lines;
			string emptyString;
			unsigned int lineCount = 0;
			lines.open(directory + tickerLower + ".us.txt");

			while (!lines.eof()) {
				getline(lines, emptyString);
				lineCount++;
			}

			if (equityType == "etf") {
				etfs[etfs.size() - 1].lengthOfDays = lineCount;
			}
			else {
				stocks[stocks.size() - 1].lengthOfDays = lineCount;
				stocks[stockLoadIndex].close.reserve(lineCount);
				stocks[stockLoadIndex].open.reserve(lineCount);
				stocks[stockLoadIndex].high.reserve(lineCount);
				stocks[stockLoadIndex].low.reserve(lineCount);
				stocks[stockLoadIndex].date.reserve(lineCount);
				stocks[stockLoadIndex].volume.reserve(lineCount);
			}

			io::CSVReader<7> in(directory + tickerLower + ".us.txt");
			try {
				in.read_header(io::ignore_extra_column, "Date", "Open", "High", "Low", "Close", "Volume", "OpenInt");
			}
			catch (...) {
				cout << "Error reading " + directory + tickerLower + ".us.txt" << endl;
			}

			int i = 0;
			for (; i < preTicker.size(); i++) {
				if (preTicker[i] == tickerUpper) {
					break;
				}
			}

			if (i < preTicker.size()) {
				if (equityType == "etf") {
					etfs[etfLoadIndex].marketCap = currentMarketCap[i];
					etfs[etfLoadIndex].industry = tickerIndustry[i];
				}
				else {
					stocks[stockLoadIndex].marketCap = currentMarketCap[i];
					stocks[stockLoadIndex].industry = tickerIndustry[i];
				}
			}

			std::string date;
			float open, high, low, close, openint;
			unsigned int volume;
			if (equityType == "etf") {
				while (in.read_row(date, open, high, low, close, volume, openint)) {
					etfs[etfLoadIndex].date.push_back(date);
					etfs[etfLoadIndex].open.push_back(open);
					etfs[etfLoadIndex].high.push_back(high);
					etfs[etfLoadIndex].low.push_back(low);
					etfs[etfLoadIndex].close.push_back(close);
					etfs[etfLoadIndex].volume.push_back(volume);
				}
			}
			else {
				while (in.read_row(date, open, high, low, close, volume, openint)) {
					stocks[stockLoadIndex].date.push_back(date);
					stocks[stockLoadIndex].open.push_back(open);
					stocks[stockLoadIndex].high.push_back(high);
					stocks[stockLoadIndex].low.push_back(low);
					stocks[stockLoadIndex].close.push_back(close);
					stocks[stockLoadIndex].volume.push_back(volume);
				}
			}

			if (equityType == "etf") {
				etfLoadIndex++;
			}
			else {
				stockLoadIndex++;
			}
		}
		_findclose(hFile);
	}
}

int main() {

	//ImageBMP bmp;
	//bmp.createBMP(4, 16777216, "test");
	//bmp.setInitialBMPColor(255, 0, 0);
	//bmp.saveBMP();

	//int** a = new int*[16777216];
	//for (unsigned long i = 0; i < 16777216; ++i)
	//	a[i] = new int[4];

	Sleep(1000);

	for (unsigned long i = 0; i < 16777216; ++i) {
		delete[] a[i];
	}
	delete[] a;

	clock_t begin = clock();

	string specificEquity = "tde.us.txt";
	specificEquity = "";

	//stock
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nasdaq stocks\\1\\", "stock", specificEquity);
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nasdaq stocks\\2\\", "stock", specificEquity);
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nyse stocks\\1\\", "stock", specificEquity);
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nyse stocks\\2\\", "stock", specificEquity);
	
	//index
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nasdaq etfs\\", "etf", specificEquity);
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nyse etfs\\", "etf", specificEquity);
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << elapsed_secs << endl;
	
	int greenRedCount = 0;
	float greenRedTotal = 0;

	string tickerHighest = "";
	float greenOverRed = 0.0;

	for (int i = 0; i < stocks.size(); i++) {

		if (stocks[i].volume.size() < 1825 || stocks[i].volume.size() > 2190) {
			continue;
		}

		//if (stocks[i].ticker != "LPLA") {
		//	continue;
		//}

		if (stocks[i].close.size() == 0) {
			continue;
		}

		if (stocks[i].close[stocks[i].close.size() - 1] < 5)
			continue;

		float green = 0;
		float red = 0;

		for (int d = 1; d < stocks[i].volume.size() - 1; d++) {
			if (stocks[i].close[d - 1] < stocks[i].open[d - 1]) {
				if (stocks[i].close[d] > stocks[i].open[d]) {
					//if (stocks[i].open[d] < stocks[i].close[d - 1]) {
						//if (stocks[i].close[d] > stocks[i].open[d - 1]) {
					if (stocks[i].low[d] < stocks[i].low[d - 1] && stocks[i].open[d] < stocks[i].close[d - 1]) {
						if (stocks[i].high[d] > stocks[i].high[d - 1] && stocks[i].close[d] > stocks[i].open[d - 1]) {
							//cout << stocks[i].date[d] << endl;
							if (stocks[i].close[d + 1] > stocks[i].open[d + 1]) {
								green++;
							}
							else {
								red++;
							}
						}
					}
				}
			}
		}

		if (green / red >= 0.0001 && green / red < 100) {
			//cout << "G/R After Bullish Engulfing is: " + to_string(green / red) << endl;
			cout << "Analyzing Stock: " + stocks[i].ticker + " G/R: " + to_string(green / red) << endl;
			greenRedCount++;
			greenRedTotal += (green / red);

			if (green / red > greenOverRed) {
				tickerHighest = stocks[i].ticker;
				greenOverRed = green / red;
			}
		}
	}

	cout << "Results: " + to_string(greenRedTotal / greenRedCount) << endl;
	cout << "Highest: " + tickerHighest + " with ratio: " + to_string(greenOverRed) << endl;

	//unsigned int totalDays = 0;
	//for (int i = 0; i < stocks.size(); i++) {
	//	totalDays += stocks[i].volume.size();
		//cout << stocks[i].ticker + ", days: " + to_string(stocks[i].volume.size()) << endl;
	//}
	
	
	system("PAUSE");
	return 0;
}