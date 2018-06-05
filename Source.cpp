#include <iostream>
#include <vector>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fstream>
#include <limits>
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

class CandleStickPattern {
public:
	vector<string> tickerAndDate;
	string candleStickType;
	string candleStickPattern;
	unsigned int numberOfOccurences;
	float profitabilityOfGivenDay;
	float measuredProfit;
	float greenDay;
	float redDay;
	//float ratioGR;

	CandleStickPattern() {
		candleStickType = "";
		candleStickPattern = "";
		numberOfOccurences = 0;
		profitabilityOfGivenDay = 0.0;
		measuredProfit = 0.0;
		greenDay = redDay = 0.0;// ratioGR = 0.0;
	}

	bool operator < (const CandleStickPattern& pattern) const
	{
		return (measuredProfit >= pattern.measuredProfit);
	}
};

vector<string> preTicker;
vector<unsigned long long> currentMarketCap;
vector<string> tickerIndustry;

vector<Equity> equities;
vector<Equity> stocks;
vector<Equity> etfs;
unsigned int stockLoadIndex = 0;
unsigned int etfLoadIndex = 0;

unsigned int loadXEquities = 0;

//unsigned int N = 5;
//vector<vector<vector<vector<vector<vector<vector<vector<vector<vector<vector<CandleStickPattern>>>>>>>>>>> candles;

void loadEODLatestData(string folderPath) {
	cout << "Loading Historical Equity Data from latest EOD..." << endl;

	string directory = folderPath;

	_finddata_t info;
	intptr_t hFile;

	folderPath = directory + "*.*";
	const char* fp = folderPath.c_str();

	if ((hFile = _findfirst(fp, &info)) != -1L) {

		_findnext(hFile, &info); //read "." hidden directory file

		while (_findnext(hFile, &info) == 0) {

			string fileName = info.name;

			io::CSVReader<7> in(directory + info.name);

			std::string ticker, date = "";
			float open, high, low, close;
			unsigned int volume;
			while (in.read_row(ticker, date, open, high, low, close, volume)) {

				if (std::find(preTicker.begin(), preTicker.end(), ticker) != preTicker.end())
				{
					for (unsigned int e = 0; e < equities.size(); e++) {
						if (equities[e].ticker == ticker) {
							date.insert(4, "-");
							date.insert(7, "-");

							equities[e].date.push_back(date);
							equities[e].close.push_back(close);
							equities[e].open.push_back(open);
							equities[e].low.push_back(low);
							equities[e].high.push_back(high);
							equities[e].volume.push_back(volume);
						}
					}
				}

			}

			//break;
		}
		_findclose(hFile);
	}
}

void loadEquityFolder(string folderPath, string equityType, string specificEquity) {
	cout << "Loading Historical Equity Data..." << endl;

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

		//int stop = 0;

		while (_findnext(hFile, &info) == 0) {
			//stop++;
			//if (stop > 10) {
			//	_findclose(hFile);
			//	return;
			//}

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
			bool errorReading = false;
			try {
				in.read_header(io::ignore_extra_column, "Date", "Open", "High", "Low", "Close", "Volume", "OpenInt");
			}
			catch (...) {
				cout << "Error reading " + directory + tickerLower + ".us.txt" << endl;
				errorReading = true;
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
				if (errorReading) {
					etfs.pop_back();
					etfLoadIndex--;
				}
				etfLoadIndex++;
			}
			else {
				if (errorReading) {
					stocks.pop_back();
					stockLoadIndex--;
				}
				stockLoadIndex++;
			}
		}
		_findclose(hFile);
	}
}

bool isOneWhiteSoldier(float o0, float c0, float o1, float c1) {
	if (c0 < o0) { //if previous day was bearish
		if (c1 > o1) { //if current day was bullish
			if (o1 > c0) { //if open of current day is greater than close of  previous
				if (c1 > o0) { //if close of first day is greater than open of previous
					return true;
				}
			}
		}
	}

	return false;
}

bool isBullishEngulfing(float o0, float c0, float o1, float c1) {
	if (c0 < o0) { //if previous day was bearish
		if (c1 > o1) { //if current day was bullish
			if (o1 < c0) { //if open of current day is less than close of  previous
				if (c1 > o0) { //if close of first day is greater than open of previous
					return true;
				}
			}
		}
	}

	return false;
}

bool isBullishHarami(float o0, float c0, float o1, float c1) {
	if (c0 < o0) { //if previous day was bearish
		if (c1 > o1) { //if current day was bullish
			if (o1 > c0) { //if open of current day is greater than close of  previous
				if (c1 < o0) { //if close of first day is less than open of previous
					return true;
				}
			}
		}
	}

	return false;
}

bool isBullishThreeLineStrike(float o0, float c0, float o1, float c1, float o2, float c2, float o3, float c3) {
	if (c0 < o0) { //if day1 was bearish
		if (c1 < o1 && c1 < c0) { //if day2 was bearish
			if (c2 < o2 && c2 < c1) { //if day3 was bearish
				if (c3 > o3) { //if day 4 was bullish
					if (c3 > o0 && o3 < c2) { // if day 4 was bullish engulfing of previous three bearish candles
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool isDojiNeutral(float o, float c, float h, float l) {
	if (abs(o - c) < h * 0.001) {
		if (o > (0.33 * (h - l)) + l && o < (0.66 * (h - l)) + l) {
			if (c > (0.33 * (h - l)) + l && c < (0.66 * (h - l)) + l) {
				return true;
			}
		}
	}

	return false;
}

bool isBullishAbandonedBaby(float o0, float c0, float o1, float c1) {
	//if the difference between open and close is less than 0.0001 of stock price then it is a doji 
	//doji classifications
	//neutral, gravestone, dragonfly



	if (c0 < o0) { //if previous day was bearish
		if (c1 > o1) { //if current day was bullish
			if (o1 > c0) { //if open of current day is greater than close of  previous
				if (c1 < o0) { //if close of first day is less than open of previous
					return true;
				}
			}
		}
	}

	return false;
}

float EMA(vector<float> arr) {
	float k = 2.0 / (arr.size());
	
	float emaYesterday = arr[0];
	float emaToday = 0.0;

	for (int e = 1; e < arr.size(); e++) {
		emaToday = (arr[e] * k) + (emaYesterday * (1.0 - k));
		emaYesterday = emaToday;
	}

	return emaToday;
}

float SMA(vector<float> arr) {
	float total = 0.0;

	for (int i = 1; i < arr.size(); i++) {
		total += arr[i];
	}

	return total / (arr.size() - 1);
}

vector<CandleStickPattern> calculateHighestProfitability(unsigned int historyLengthOfCandles, unsigned int givenDay, bool profitUpToGivenDay, unsigned int howManyTopPatterns, string typeOfPattern, unsigned int dividers) {
	cout << "Analyzing Equities with pattern division: " << to_string(dividers) << endl;
	
	//givenDay = 0 = gap up/down percentage
	//giveDay > 0 = profit / loss percentage of given day
	vector<CandleStickPattern> topPatterns;
	vector<CandleStickPattern> foundPatterns;

	unsigned int lowestNumberOfOccurences = UINT_MAX;
	unsigned int highestNumberOfOccurences = 0;
	float highestProfitabilityOfGivenDay = 0.0;

	for (unsigned int i = 0; i < equities.size(); i++) {
		if (equities[i].close.at(equities[i].close.size() - 1) <= 1.00)
			continue;
		
		if (equities[i].volume.size() <= givenDay)
			continue;

		//cout << "Analyzing bullish on: " + equities[i].ticker << endl;// +" on date:" + dateOfBullishEngulfing << endl;
		//cout << "FoundPattern size: " + to_string(foundPatterns.size()) << endl;
		/*
		vector<float> SMA;
		vector<float> rsiUpDay;
		vector<float> rsiDownDay;
		vector<float> rsiSMA;
		vector<float> rsiEMA;
		*/
		for (int d = historyLengthOfCandles; d < equities[i].volume.size() - 1 - givenDay; d++) {
			//string dateOfBullishEngulfing = equities[i].date[d];
			
			if (d < historyLengthOfCandles)
				continue;

			if (equities[i].date[d] == "2018-05-07") {
				d += 3;
				continue;
			}

			/*
			if (d == 14) {

				float total = 0.0;
				for (int s = 0; s < 14; s++) {
					total += equities[i].close[s];
				}
				total /= 14.0;

				SMA.push_back(total);
				
			}

			if (d > 14) {
				//float total = 0.0;
				//for (int s = d - 14; s < d; s++) {
				//	total += equities[i].close[s];
				//}
				//total /= 14.0;

				SMA.push_back((SMA[SMA.size() - 1] * 13.0 + equities[i].close[d]) / 14.0);

				if (SMA.size() > 14) {
					SMA.erase(SMA.begin());
				}
			}*/

			//rsi SMA
			/*
			if (d == 14) {

				for (int s = 1; s < 15; s++) {
					if (equities[i].close[s] > equities[i].close[s - 1]) {
						rsiUpDay.push_back(equities[i].close[s] - equities[i].close[s - 1]);
						rsiDownDay.push_back(0.0);
					}
					else if (equities[i].close[s - 1] > equities[i].close[s]) {
						rsiUpDay.push_back(0.0);
						rsiDownDay.push_back(equities[i].close[s - 1] - equities[i].close[s]);
					}
					else {
						rsiUpDay.push_back(0.0);
						rsiDownDay.push_back(0.0);
					}
				}
				
				float averageRsiUpDay = 0.0;
				float averageRsiDownDay = 0.0;
				for (int total = 0; total < rsiUpDay.size(); total++) {
					averageRsiUpDay += rsiUpDay[total];
					averageRsiDownDay += rsiDownDay[total];
				}

				averageRsiUpDay /= 14.0;
				averageRsiDownDay /= 14.0;

				rsiSMA.push_back(100.0 - (100.0 / (1.0 + (averageRsiUpDay / averageRsiDownDay))));

			}

			if (d > 14) {

				float averageRsiUpDay = 0.0;
				float averageRsiDownDay = 0.0;
				for (int total = 0; total < rsiUpDay.size(); total++) {
					averageRsiUpDay += rsiUpDay[total];
					averageRsiDownDay += rsiDownDay[total];
				}

				averageRsiUpDay /= 14.0;
				averageRsiDownDay /= 14.0;

				float newRsiUpDayValue = 0.0;
				float newRsiDownDayValue = 0.0;

				if (equities[i].close[d] > equities[i].close[d - 1]) {
					//newRsiUpDayValue = ((averageRsiUpDay * 13.0) + (equities[i].close[d] - equities[i].close[d - 1])) / 14.0;
					//newRsiDownDayValue = ((averageRsiDownDay * 13.0) + (0.0)) / 14.0;

					//rsiUpDay.push_back(newRsiUpDayValue);
					//rsiDownDay.push_back(newRsiDownDayValue);
					newRsiUpDayValue = equities[i].close[d] - equities[i].close[d - 1];
					newRsiDownDayValue = 0.0;

					rsiUpDay.push_back(newRsiUpDayValue);
					rsiDownDay.push_back(newRsiDownDayValue);
				}
				else if (equities[i].close[d - 1] > equities[i].close[d]) {
					//newRsiDownDayValue = ((averageRsiDownDay * 13.0) + (equities[i].close[d - 1] - equities[i].close[d])) / 14.0;
					//newRsiUpDayValue = ((averageRsiUpDay * 13.0) + (0.0)) / 14.0;

					//rsiUpDay.push_back(newRsiDownDayValue);
					//rsiDownDay.push_back(newRsiDownDayValue);

					newRsiDownDayValue = equities[i].close[d - 1] - equities[i].close[d];
					newRsiUpDayValue = 0.0;

					rsiUpDay.push_back(newRsiUpDayValue);
					rsiDownDay.push_back(newRsiDownDayValue);
				}
				else {
					//newRsiUpDayValue = ((averageRsiUpDay * 13.0) + (0.0)) / 14.0;
					//newRsiDownDayValue = ((averageRsiDownDay * 13.0) + (0.0)) / 14.0;

					newRsiUpDayValue = 0.0;
					newRsiDownDayValue = 0.0;

					rsiUpDay.push_back(newRsiUpDayValue);
					rsiDownDay.push_back(newRsiDownDayValue);
				}

				if (rsiUpDay.size() > 14)
					rsiUpDay.erase(rsiUpDay.begin());
				if (rsiDownDay.size() > 14)
					rsiDownDay.erase(rsiDownDay.begin());

				averageRsiUpDay = 0.0;
				averageRsiDownDay = 0.0;
				for (int total = 0; total < rsiUpDay.size(); total++) {
					averageRsiUpDay += rsiUpDay[total];
					averageRsiDownDay += rsiDownDay[total];
				}

				averageRsiUpDay /= 14.0;
				averageRsiDownDay /= 14.0;

				float newValue = 100.0 - (100.0 / (1.0 + (averageRsiUpDay / averageRsiDownDay)));

				rsiSMA.push_back(newValue);

				if (rsiSMA.size() > 14) {
					rsiSMA.erase(rsiSMA.begin());
				}
			}
			

			//rsi EMA
			/*
			if (d == 14) {

				for (int s = 1; s < 15; s++) {
					if (equities[i].close[s] > equities[i].close[s - 1]) {
						rsiUpDay.push_back(equities[i].close[s] - equities[i].close[s - 1]);
						rsiDownDay.push_back(0.0);
					}
					else if (equities[i].close[s - 1] > equities[i].close[s]) {
						rsiUpDay.push_back(0.0);
						rsiDownDay.push_back(equities[i].close[s - 1] - equities[i].close[s]);
					}
					else {
						rsiUpDay.push_back(0.0);
						rsiDownDay.push_back(0.0);
					}
				}

				float averageRsiUpDay = 0.0;
				float averageRsiDownDay = 0.0;
				for (int total = 0; total < rsiUpDay.size(); total++) {
					averageRsiUpDay += rsiUpDay[total];
					averageRsiDownDay += rsiDownDay[total];
				}

				averageRsiUpDay /= 14.0;
				averageRsiDownDay /= 14.0;

				rsiEMA.push_back(100.0 - (100.0 / (1.0 + (averageRsiUpDay / averageRsiDownDay))));

			}

			if (d > 14) {

				float averageRsiUpDay = 0.0;
				float averageRsiDownDay = 0.0;
				for (int total = 0; total < rsiUpDay.size(); total++) {
					averageRsiUpDay += rsiUpDay[total];
					averageRsiDownDay += rsiDownDay[total];
				}

				averageRsiUpDay /= 14.0;
				averageRsiDownDay /= 14.0;

				float newRsiUpDayValue = 0.0;
				float newRsiDownDayValue = 0.0;

				float a = 2.0 / (15.0);

				if (equities[i].close[d] > equities[i].close[d - 1]) {
					newRsiUpDayValue = (( a * (equities[i].close[d] - equities[i].close[d - 1])) + ((1.0 - a) * averageRsiUpDay));
					newRsiDownDayValue = ((1.0 - a) * averageRsiDownDay);

					rsiUpDay.push_back(newRsiUpDayValue);
					rsiDownDay.push_back(newRsiDownDayValue);
				}
				else if (equities[i].close[d - 1] > equities[i].close[d]) {
					newRsiDownDayValue = ((a * (equities[i].close[d - 1] - equities[i].close[d])) + ((1.0 - a) * averageRsiDownDay));
					newRsiUpDayValue = ((1.0 - a) * averageRsiUpDay);

					rsiUpDay.push_back(newRsiDownDayValue);
					rsiDownDay.push_back(newRsiDownDayValue);
				}
				else {
					newRsiUpDayValue = ((1.0 - a) * averageRsiUpDay);
					newRsiDownDayValue = ((1.0 - a) * averageRsiDownDay);

					rsiUpDay.push_back(newRsiUpDayValue);
					rsiDownDay.push_back(newRsiDownDayValue);
				}

				if (rsiUpDay.size() > 14)
					rsiUpDay.erase(rsiUpDay.begin());
				if (rsiDownDay.size() > 14)
					rsiDownDay.erase(rsiDownDay.begin());

				float newValue = 100.0 - (100.0 / (1.0 + (newRsiUpDayValue / newRsiDownDayValue)));

				rsiEMA.push_back(newValue);

				if (rsiEMA.size() > 14) {
					rsiEMA.erase(rsiEMA.begin());
				}
			}*/

			if (typeOfPattern == "bullish engulfing") {

				if (!isBullishEngulfing(equities[i].open[d - 1], equities[i].close[d - 1], equities[i].open[d], equities[i].close[d])) {
					continue;
				}
			}

			if (typeOfPattern == "bullish harami") {
				if (!isBullishHarami(equities[i].open[d - 1], equities[i].close[d - 1], equities[i].open[d], equities[i].close[d])) {
					continue;
				}
			}

			if (typeOfPattern == "one white soldier") {
				if (!isOneWhiteSoldier(equities[i].open[d - 1], equities[i].close[d - 1], equities[i].open[d], equities[i].close[d])) {
					continue;
				}
			}

			//if (typeOfPattern == "abandoned baby") {

			//}

			//if (typeOfPattern == "three line strike") {
			//	if (!isBullishThreeLineStrike(equities[i].open[d - 3], equities[i].close[d - 3], equities[i].open[d - 2], equities[i].close[d - 2], 
			//		equities[i].open[d - 1], equities[i].close[d - 1], equities[i].open[d], equities[i].close[d])) {
			//		continue;
			//	}
			//}
			///if (rsiSMA.size() > 0)
				//if (rsiSMA[rsiSMA.size() - 1] > 70)
					//continue;

			//if (equities[i].volume[d] < equities[i].volume[d - 1])
				//continue;

			//string dateOfBullishEngulfing = equities[i].date[d];
			/*
			float N = 14.0;

			if (d < N + 1)
				continue;

			vector<float> upDays;
			vector<float> downDays;
			vector<float> closingPriceEMA;

			N = N + 1;
			for (int v = d; v > d - N; v--) {
						
				if (equities[i].close[v] > equities[i].close[v - 1]) {
					upDays.push_back(equities[i].close[v] - equities[i].close[v - 1]);
					downDays.push_back(0.0);
				}
				else if(equities[i].close[v - 1] > equities[i].close[v]){
					upDays.push_back(0.0);
					downDays.push_back(equities[i].close[v - 1] - equities[i].close[v]);
				}
				else {
					upDays.push_back(0.0);
					downDays.push_back(0.0);
				}

				closingPriceEMA.push_back(equities[i].close[v]);
			}

			std::reverse(closingPriceEMA.begin(), closingPriceEMA.end());
			std::reverse(upDays.begin(), upDays.end());
			std::reverse(downDays.begin(), downDays.end());

					
					
			float closePriceEma = EMA(closingPriceEMA);

			float emaUpDays = SMA(upDays);
			float emaDownDays = SMA(downDays);

			float rsi = 100.0 - (100.0 / (1.0 + (emaUpDays / emaDownDays)));
			*/

			//int stop = 0;

			CandleStickPattern pattern;

			//parse pattern into string
			string extractedPattern = "";

			float top = 0.0;
			for (int t = historyLengthOfCandles; t >= 0; t--) {
				float temp = max(max(max(equities[i].open[d - t], equities[i].close[d - t]), equities[i].low[d - t]), equities[i].high[d - t]);
				if (temp > top)
					top = temp;
			}
					
			float bottom = FLT_MAX;
			for (int b = historyLengthOfCandles; b >= 0; b--) {
				float temp = min(min(min(equities[i].open[d - b], equities[i].close[d - b]), equities[i].low[d - b]), equities[i].high[d - b]);
				if (temp < bottom)
					bottom = temp;
			}

			float difference = top - bottom;
					
			for (int h = historyLengthOfCandles; h >= 0; h--) {
				//int remainder = (equities[i].low[d - h] - bottom) / (difference / dividers);
				//remainder = (remainder == dividers) ? (dividers - 1) : (remainder);
				//extractedPattern.append(to_string(remainder) + ",");
				//remainder = (equities[i].high[d - h] - bottom) / (difference / dividers);
				//remainder = (remainder == dividers) ? (dividers - 1) : (remainder);
				//extractedPattern.append(to_string(remainder) + ",");
				int remainder = (equities[i].open[d - h] - bottom) / (difference / dividers);
				remainder = (remainder == dividers) ? (dividers - 1) : (remainder);
				extractedPattern.append(to_string(remainder) + "|");
				remainder = (equities[i].close[d - h] - bottom) / (difference / dividers);
				remainder = (remainder == dividers) ? (dividers - 1) : (remainder);
				if (h > 0)
					extractedPattern.append(to_string(remainder) + "|");
				else
					extractedPattern.append(to_string(remainder));
			}


			//"l,h,o,c,l,h,o,c,l,h,o,c,l,h,o,c"
					
			//check if pattern has already been recorded
			unsigned int f = 0;
			for (; f < foundPatterns.size(); f++) {
				if (foundPatterns[f].candleStickPattern == extractedPattern) {
					//if (foundPatterns[f].numberOfOccurences < lowestNumberOfOccurences)
					//	lowestNumberOfOccurences = foundPatterns[f].numberOfOccurences;
					//if (foundPatterns[f].numberOfOccurences > highestNumberOfOccurences)
					//	highestNumberOfOccurences = foundPatterns[f].numberOfOccurences;

					//+1 to pattern number of occurences and add profitability of givenDay
					float addProfitToDate = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
					if (isnan(addProfitToDate))
						addProfitToDate = 0.0;

					foundPatterns[f].tickerAndDate.push_back(equities[i].ticker + ":" + equities[i].date[d] + ":" + to_string(addProfitToDate));
					foundPatterns[f].numberOfOccurences++;
					if (givenDay > 0) {
						if (profitUpToGivenDay == false) {
							//if (((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay]) > highestProfitabilityOfGivenDay)
								//highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];

							foundPatterns[f].profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
							if ((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay] > 0.0) {
								foundPatterns[f].greenDay += 1.0;
							}
							else {
								foundPatterns[f].redDay += 1.0;
							}
						}
						//else {
						//	if (((equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1]) > highestProfitabilityOfGivenDay)
						//		highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];

						//	foundPatterns[f].profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];
						//}
					}
					//else {
					//	if (((equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]) > highestProfitabilityOfGivenDay)
					//		highestProfitabilityOfGivenDay = (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d];
								
					//	foundPatterns[f].profitabilityOfGivenDay += (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]; //calculate profit of gap day
					//}
					break;
				}

			}

			//if did not find pattern add it as new one
					
			if (f >= foundPatterns.size()) {

				//if (foundPatterns[f].numberOfOccurences < lowestNumberOfOccurences)
				//	lowestNumberOfOccurences = foundPatterns[f].numberOfOccurences;
				//if (foundPatterns[f].numberOfOccurences > highestNumberOfOccurences)
				//	highestNumberOfOccurences = foundPatterns[f].numberOfOccurences;

				CandleStickPattern c;

				float addProfitToDate = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
				if (isnan(addProfitToDate))
					addProfitToDate = 0.0;

				c.tickerAndDate.push_back(equities[i].ticker + ":" + equities[i].date[d] + ":" + to_string(addProfitToDate));
				c.candleStickPattern = extractedPattern;
				c.numberOfOccurences = 1;
				if (givenDay > 0) {
					if (profitUpToGivenDay == false) {
						//if (((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay]) > highestProfitabilityOfGivenDay)
							//highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];

						c.profitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
						if ((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay] > 0.0) {
							c.greenDay += 1.0;
						}
						else {
							c.redDay += 1.0;
						}
					}
					else {
						//if (((equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1]) > highestProfitabilityOfGivenDay)
							//highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];

						c.profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];
					}
				}
				//else {
				//	if (((equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]) > highestProfitabilityOfGivenDay)
				//		highestProfitabilityOfGivenDay = (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d];

				//	c.profitabilityOfGivenDay = (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]; //calculate profit of gap day
				//}
				foundPatterns.push_back(c);
			}

		}
	}

	//average out the profit and days found of patterns
	for (unsigned int a = 0; a < foundPatterns.size(); a++) {
		foundPatterns[a].profitabilityOfGivenDay = foundPatterns[a].profitabilityOfGivenDay / float(foundPatterns[a].numberOfOccurences);
	}

	//pick top howManyTopPatterns
	//float differenceNumberOfOccurences = 1 + highestNumberOfOccurences - lowestNumberOfOccurences;

	for (unsigned int p = 0; p < foundPatterns.size(); p++) {

		if (foundPatterns[p].profitabilityOfGivenDay <= 0.5)
			continue;


		if (foundPatterns[p].numberOfOccurences == 1) {
			float profit = (foundPatterns[p].profitabilityOfGivenDay) * (foundPatterns[p].greenDay / foundPatterns[p].redDay);
			foundPatterns[p].measuredProfit = profit;
			continue;
		}

		if (foundPatterns[p].numberOfOccurences < 30)
			continue;

		if (foundPatterns[p].greenDay / foundPatterns[p].redDay < 2.0)
			continue;

		float numberOfOccurences = foundPatterns[p].numberOfOccurences;// -lowestNumberOfOccurences;
		if (numberOfOccurences == 0)
			numberOfOccurences = 1;
		//float profit = (numberOfOccurences / differenceNumberOfOccurences) * (foundPatterns[p].profitabilityOfGivenDay / highestProfitabilityOfGivenDay);
		//float profit = numberOfOccurences * foundPatterns[p].profitabilityOfGivenDay;
		//float profit = numberOfOccurences;
		//float profit = foundPatterns[p].profitabilityOfGivenDay;
		float profit = (foundPatterns[p].profitabilityOfGivenDay) * (foundPatterns[p].greenDay / foundPatterns[p].redDay);
		foundPatterns[p].measuredProfit = profit;

		topPatterns.push_back(foundPatterns[p]);

		/*
		if (topPatterns.size() == 0) {
			topPatterns.push_back(foundPatterns[p]);
			continue;
		}

		unsigned int t = 0;
		for (; t < topPatterns.size(); t++) {
			//insert before beginning element if bigger than it
			if (profit > topPatterns[t].measuredProfit && t == 0) {
				topPatterns.insert(topPatterns.begin(), foundPatterns[p]);
				break;
			}

			if (topPatterns.size() == 1) {
				if (profit >= topPatterns[0].measuredProfit) {
					topPatterns.insert(topPatterns.begin(), foundPatterns[p]);
				}
				else {
					topPatterns.push_back(foundPatterns[p]);
				}
				break;
			}

			if (t == topPatterns.size() - 1) {
				if (profit >= topPatterns[t - 1].measuredProfit) {
					topPatterns.insert(topPatterns.begin() + t, foundPatterns[p]);
				}
				else {
					topPatterns.push_back(foundPatterns[p]);
				}
				break;
			}

			if (profit < topPatterns[t].measuredProfit && profit >= topPatterns[t + 1].measuredProfit) {
				topPatterns.insert(topPatterns.begin() + t + 1, foundPatterns[p]);
				break;
			}
		}*/

		std::sort(topPatterns.begin(), topPatterns.end());

		//if (topPatterns.size() > 100)
			//topPatterns.pop_back();


	}

	for (unsigned int t = 0; t < topPatterns.size(); t++) {

		if (typeOfPattern == "bullish engulfing") {
			topPatterns[t].candleStickType = "bullish engulfing";
		}

		if (typeOfPattern == "bullish harami") {
			topPatterns[t].candleStickType = "bullish harami";
		}

		if (typeOfPattern == "one white soldier") {
			topPatterns[t].candleStickType = "one white soldier";
		}
	}

	return topPatterns;
}

float simulation(string startDate, string endDate, float initialDeposit, vector<CandleStickPattern> &candlestickMetric, int tradingMethod) {
	
	
	return 0.0;	
}

int main() {

	//create a slider
	// low number of occurrences -------       number of occurrences       ---------- max number

	//rsi will affect G/R ratio: if rsi is 40 or below, on a certain range, is the next day green or red?
	//rsi might affect how much profitability 
	//average volume
	//price within certain range
	//success percentage that profit reached average predicted range
	//percentage your trades were positive

	//when testing, don't buy in if volume < 1,000,000

	string specificEquity = "fb.us.txt";
	specificEquity = "";

	//stock
	loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nasdaq stocks\\1\\", "stock", specificEquity);
	loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nasdaq stocks\\2\\", "stock", specificEquity);
	loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nyse stocks\\1\\", "stock", specificEquity);
	loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nyse stocks\\2\\", "stock", specificEquity);

	//index
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nasdaq etfs\\", "etf", specificEquity);
	//loadEquityFolder("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\us\\nyse etfs\\", "etf", specificEquity);
	
	equities.insert(equities.end(), stocks.begin(), stocks.end());
	equities.insert(equities.end(), etfs.begin(), etfs.end());

	loadEODLatestData("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\eodNYSE\\");
	loadEODLatestData("C:\\Users\\benlo\\Documents\\Stock Data\\data\\daily\\eodNASDAQ\\");

	int averageDays = 0;
	for (int d = 0; d < equities.size(); d++) {
		averageDays += equities[d].volume.size();
	}

	averageDays /= equities.size(); //2092

	cout << "average stock lifetime in days: " << averageDays << endl;
	
	CandleStickPattern bestPatternWithSettings;
	/*
	int recordI = 1;
	int recordJ = 8;

	for (int i = 1; i < 5; i++) {
		for (int j = 8; j < 10; j++) {
			cout << "days going back: " + to_string(i) + ", divided into: " + to_string(j) << endl;
			vector<CandleStickPattern> top100BullishEngulfing = calculateHighestProfitability(i, 1, false, 100, "bullish engulfing", j);
			if (top100BullishEngulfing.size() > 0) {

				cout << "best candlestick measured profit of this combination: " + to_string(top100BullishEngulfing[0].measuredProfit) << endl;

				if (top100BullishEngulfing[0].measuredProfit > bestPatternWithSettings.measuredProfit) {
					bestPatternWithSettings = top100BullishEngulfing[0];
					recordI = i;
					recordJ = j;
				}
			}
		}
	}

	vector<CandleStickPattern> top100BullishEngulfing = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 8);

	cout << "occurences: " + to_string(bestPatternWithSettings.numberOfOccurences) + ", average profit: " +
		to_string(bestPatternWithSettings.profitabilityOfGivenDay) + ", totalProfit: " +
		to_string(bestPatternWithSettings.measuredProfit)
		+ ", G/R ratio: " + to_string(bestPatternWithSettings.greenDay / bestPatternWithSettings.redDay) << endl;

	cout << "days going back: " + to_string(recordI) + ", divided into: " + to_string(recordJ) << endl;
	*/

	

	vector<CandleStickPattern> topBullishEngulfing7 = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 7);
	vector<CandleStickPattern> topBullishEngulfing8 = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 8);
	vector<CandleStickPattern> topBullishEngulfing9 = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 9);
	vector<CandleStickPattern> topBullishEngulfing10 = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 10);
	vector<CandleStickPattern> topBullishEngulfing11 = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 11);

	vector<CandleStickPattern> topBullishHarami7 = calculateHighestProfitability(2, 1, false, 100, "bullish harami", 7);
	vector<CandleStickPattern> topBullishHarami8 = calculateHighestProfitability(2, 1, false, 100, "bullish harami", 8);
	vector<CandleStickPattern> topBullishHarami9 = calculateHighestProfitability(2, 1, false, 100, "bullish harami", 9);
	vector<CandleStickPattern> topBullishHarami10 = calculateHighestProfitability(2, 1, false, 100, "bullish harami", 10);
	vector<CandleStickPattern> topBullishHarami11 = calculateHighestProfitability(2, 1, false, 100, "bullish harami", 11);

	vector<CandleStickPattern> topBullishOneWhiteSoldier7 = calculateHighestProfitability(2, 1, false, 100, "one white soldier", 7);
	vector<CandleStickPattern> topBullishOneWhiteSoldier8 = calculateHighestProfitability(2, 1, false, 100, "one white soldier", 8);
	vector<CandleStickPattern> topBullishOneWhiteSoldier9 = calculateHighestProfitability(2, 1, false, 100, "one white soldier", 9);
	vector<CandleStickPattern> topBullishOneWhiteSoldier10 = calculateHighestProfitability(2, 1, false, 100, "one white soldier", 10);
	vector<CandleStickPattern> topBullishOneWhiteSoldier11 = calculateHighestProfitability(2, 1, false, 100, "one white soldier", 11);

	vector<CandleStickPattern> finalResults;
	finalResults.insert(finalResults.end(), topBullishEngulfing7.begin(), topBullishEngulfing7.end());
	finalResults.insert(finalResults.end(), topBullishEngulfing8.begin(), topBullishEngulfing8.end());
	finalResults.insert(finalResults.end(), topBullishEngulfing9.begin(), topBullishEngulfing9.end());
	finalResults.insert(finalResults.end(), topBullishEngulfing10.begin(), topBullishEngulfing10.end());
	finalResults.insert(finalResults.end(), topBullishEngulfing11.begin(), topBullishEngulfing11.end());
	finalResults.insert(finalResults.end(), topBullishHarami7.begin(), topBullishHarami7.end());
	finalResults.insert(finalResults.end(), topBullishHarami8.begin(), topBullishHarami8.end());
	finalResults.insert(finalResults.end(), topBullishHarami9.begin(), topBullishHarami9.end());
	finalResults.insert(finalResults.end(), topBullishHarami10.begin(), topBullishHarami10.end());
	finalResults.insert(finalResults.end(), topBullishHarami11.begin(), topBullishHarami11.end());
	finalResults.insert(finalResults.end(), topBullishOneWhiteSoldier7.begin(), topBullishOneWhiteSoldier7.end());
	finalResults.insert(finalResults.end(), topBullishOneWhiteSoldier8.begin(), topBullishOneWhiteSoldier8.end());
	finalResults.insert(finalResults.end(), topBullishOneWhiteSoldier9.begin(), topBullishOneWhiteSoldier9.end());
	finalResults.insert(finalResults.end(), topBullishOneWhiteSoldier10.begin(), topBullishOneWhiteSoldier10.end());
	finalResults.insert(finalResults.end(), topBullishOneWhiteSoldier11.begin(), topBullishOneWhiteSoldier11.end());

	cout << "Sorting results..." << endl;
	std::sort(finalResults.begin(), finalResults.end());

	ofstream fileResults;
	fileResults.open("results.txt");
	fileResults << "MP,G|R,AverageProfit,Occurences,Pattern,Type" << endl;
	for (unsigned int r = 0; r < finalResults.size(); r++) {
		fileResults << to_string(finalResults[r].measuredProfit) << "," << to_string(finalResults[r].greenDay / finalResults[r].redDay) << "," <<
		to_string(finalResults[r].profitabilityOfGivenDay) << "," << to_string(finalResults[r].numberOfOccurences) << "," <<
		finalResults[r].candleStickPattern << "," << finalResults[r].candleStickType << endl;
	}

	fileResults.close();
	

	/*
	//TESTING
	vector<CandleStickPattern> calibratedCandlePatterns;

	io::CSVReader<6> in("results.txt");
	float mp, gr, ap, occ = 0.0;
	string pattern, type = "";
	in.read_header(io::ignore_extra_column, "MP", "G|R", "AverageProfit", "Occurences", "Pattern", "Type");
	while (in.read_row(mp, gr, ap, occ, pattern, type)) {
		CandleStickPattern c;
		c.candleStickPattern = pattern;
		c.candleStickType = type;
		c.greenDay = gr;
		c.redDay = gr;
		c.measuredProfit = mp;
		c.numberOfOccurences = occ;
		c.profitabilityOfGivenDay = ap;
		
		calibratedCandlePatterns.push_back(c);
	}
	*/


	/*
	for (int i = 0; i < topBullishEngulfing7.size(); i++) {
		cout << topBullishEngulfing7[i].candleStickType << endl;
		cout << "occurences: " + to_string(topBullishEngulfing7[i].numberOfOccurences) + ", average profit: " +
			to_string(topBullishEngulfing7[i].profitabilityOfGivenDay) + ", totalProfit: " +
			to_string(topBullishEngulfing7[i].measuredProfit)
			+ ", G/R ratio: " + to_string(topBullishEngulfing7[i].greenDay / topBullishEngulfing7[i].redDay) << endl;
	}
	*/

	/*
	for (int i = 0; i < topBullishEngulfing7.size(); i++) {
		if (topBullishEngulfing7.size() > 0) {
			for (int j = 0; j < topBullishEngulfing7[i].tickerAndDate.size(); j++) {
				cout << topBullishEngulfing7[i].tickerAndDate[j] << endl;
			}
		}
	}
	*/

	/*
	clock_t begin = clock();
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << elapsed_secs << endl;*/
	/*
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
	*/
	
	system("PAUSE");
	return 0;
}
