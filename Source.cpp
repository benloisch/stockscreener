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
	unsigned int numberOfOccurences;
	float profitabilityOfGivenDay;
	float measuredProfit;
	//float greenDay;
	//float redDay;
	//float ratioGR;

	CandleStickPattern() {
		candleStickType = "";
		numberOfOccurences = 0;
		profitabilityOfGivenDay = 0.0;
		measuredProfit = 0.0;
		//greenDay = redDay = ratioGR = 0.0;
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
			//if (stop > 100) {
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

vector<CandleStickPattern> calculateHighestProfitability(unsigned int historyLengthOfCandles, unsigned int givenDay, bool profitUpToGivenDay, unsigned int howManyTopPatterns, string typeOfPattern, unsigned int dividers) {
	cout << "Analyzing Equities..." << endl;
	
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

		for (int d = historyLengthOfCandles; d < equities[i].volume.size() - 1 - givenDay; d++) {
			//string dateOfBullishEngulfing = equities[i].date[d];
			
			if (typeOfPattern == "bullish engulfing") {
				
				if (isBullishEngulfing(equities[i].open[d - 1], equities[i].close[d - 1], equities[i].open[d], equities[i].close[d])) {
					
					string dateOfBullishEngulfing = equities[i].date[d];

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
						extractedPattern.append(to_string(remainder) + ",");
						remainder = (equities[i].close[d - h] - bottom) / (difference / dividers);
						remainder = (remainder == dividers) ? (dividers - 1) : (remainder);
						if (h > 0)
							extractedPattern.append(to_string(remainder) + ",");
						else
							extractedPattern.append(to_string(remainder));
					}


					//"l,h,o,c,l,h,o,c,l,h,o,c,l,h,o,c"
					
					//check if pattern has already been recorded
					unsigned int f = 0;
					for (; f < foundPatterns.size(); f++) {
						if (foundPatterns[f].candleStickType == extractedPattern) {
							if (foundPatterns[f].numberOfOccurences < lowestNumberOfOccurences)
								lowestNumberOfOccurences = foundPatterns[f].numberOfOccurences;
							if (foundPatterns[f].numberOfOccurences > highestNumberOfOccurences)
								highestNumberOfOccurences = foundPatterns[f].numberOfOccurences;

							//+1 to pattern number of occurences and add profitability of givenDay
							foundPatterns[f].tickerAndDate.push_back(equities[i].ticker + ":" + equities[i].date[d]);
							foundPatterns[f].numberOfOccurences++;
							if (givenDay > 0) {
								if (profitUpToGivenDay == false) {
									if (((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay]) > highestProfitabilityOfGivenDay)
										highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];

									foundPatterns[f].profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
								}
								else {
									if (((equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1]) > highestProfitabilityOfGivenDay)
										highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];

									foundPatterns[f].profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];
								}
							}
							else {
								if (((equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]) > highestProfitabilityOfGivenDay)
									highestProfitabilityOfGivenDay = (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d];
								
								foundPatterns[f].profitabilityOfGivenDay += (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]; //calculate profit of gap day
							}
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
						c.tickerAndDate.push_back(equities[i].ticker + ":" + equities[i].date[d]);
						c.candleStickType = extractedPattern;
						c.numberOfOccurences = 1;
						if (givenDay > 0) {
							if (profitUpToGivenDay == false) {
								if (((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay]) > highestProfitabilityOfGivenDay)
									highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];

								c.profitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
							}
							else {
								if (((equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1]) > highestProfitabilityOfGivenDay)
									highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];

								c.profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + 1]) / equities[i].open[d + 1];
							}
						}
						else {
							if (((equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]) > highestProfitabilityOfGivenDay)
								highestProfitabilityOfGivenDay = (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d];

							c.profitabilityOfGivenDay = (equities[i].open[d + 1] - equities[i].close[d]) / equities[i].close[d]; //calculate profit of gap day
						}
						foundPatterns.push_back(c);
					}

				
				}
			}
		}
	}

	//average out the profit and days found of patterns
	for (unsigned int a = 0; a < foundPatterns.size(); a++) {
		foundPatterns[a].profitabilityOfGivenDay = foundPatterns[a].profitabilityOfGivenDay / float(foundPatterns[a].numberOfOccurences);
	}

	//pick top howManyTopPatterns
	float differenceNumberOfOccurences = 1 + highestNumberOfOccurences - lowestNumberOfOccurences;

	for (unsigned int p = 0; p < foundPatterns.size(); p++) {
		if (foundPatterns[p].profitabilityOfGivenDay <= 0.0)
			continue;

		if (foundPatterns[p].numberOfOccurences == 1)
			continue;

		if (foundPatterns[p].numberOfOccurences < 30)
			continue;

		float numberOfOccurences = foundPatterns[p].numberOfOccurences;// -lowestNumberOfOccurences;
		if (numberOfOccurences == 0)
			numberOfOccurences = 1;
		//float profit = (numberOfOccurences / differenceNumberOfOccurences) * (foundPatterns[p].profitabilityOfGivenDay / highestProfitabilityOfGivenDay);
		//float profit = numberOfOccurences * foundPatterns[p].profitabilityOfGivenDay;
		//float profit = numberOfOccurences;
		float profit = foundPatterns[p].profitabilityOfGivenDay;
		foundPatterns[p].measuredProfit = profit;

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
		}



		//if (topPatterns.size() > 100)
			//topPatterns.pop_back();
		

	}

	return topPatterns;
}

int main() {
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

	vector<CandleStickPattern> top100BullishEngulfing = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 9);

	for (int i = 0; i < top100BullishEngulfing.size(); i++) {
		cout << top100BullishEngulfing[i].candleStickType << endl;
		cout << "occurences: " + to_string(top100BullishEngulfing[i].numberOfOccurences) + ", average profit: " + 
			to_string(top100BullishEngulfing[i].profitabilityOfGivenDay) + ", totalProfit: " + to_string(top100BullishEngulfing[i].numberOfOccurences  * top100BullishEngulfing[i].profitabilityOfGivenDay) << endl;
	}
	/*
	for (int j = 0; j < top100BullishEngulfing[0].tickerAndDate.size(); j++) {
		cout << top100BullishEngulfing[0].tickerAndDate[j] << endl;
	}*/

	//initialize 48828125 candlestick combinations
	/*
	candles.resize(N);
	for (unsigned int a = 0; a < N; a++) {
		candles[a].resize(N);
		for (unsigned int b = 0; b < N; b++) {
			candles[a][b].resize(N);
			for (unsigned int c = 0; c < N; c++) {
				candles[a][b][c].resize(N);
				for (unsigned int d = 0; d < N; d++) {
					candles[a][b][c][d].resize(N);
					for (unsigned int e = 0; e < N; e++) {
						candles[a][b][c][d][e].resize(N);
						for (unsigned int f = 0; f < N; f++) {
							candles[a][b][c][d][e][f].resize(N);
							for (unsigned int g = 0; g < N; g++) {
								candles[a][b][c][d][e][f][g].resize(N);
								for (unsigned int h = 0; h < N; h++) {
									candles[a][b][c][d][e][f][g][h].resize(N);
									for (unsigned int i = 0; i < N; i++) {
										candles[a][b][c][d][e][f][g][h][i].resize(N);
										for (unsigned int j = 0; j < N; j++) {
											candles[a][b][c][d][e][f][g][h][i][j].resize(N);
										}}}}}}}}}}
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