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
	float greenDay;
	float redDay;
	//float ratioGR;

	CandleStickPattern() {
		candleStickType = "";
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

		vector<float> SMA;
		vector<float> rsiUpDay;
		vector<float> rsiDownDay;
		vector<float> rsiSMA;
		vector<float> rsiEMA;

		for (int d = historyLengthOfCandles; d < equities[i].volume.size() - 1 - givenDay; d++) {
			//string dateOfBullishEngulfing = equities[i].date[d];
			
			if (d < 13)
				continue;

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

				if (isBullishEngulfing(equities[i].open[d - 1], equities[i].close[d - 1], equities[i].open[d], equities[i].close[d])) {

					///if (rsiSMA.size() > 0)
						//if (rsiSMA[rsiSMA.size() - 1] > 70)
							//continue;

					//if (equities[i].volume[d] < equities[i].volume[d - 1])
						//continue;

					string dateOfBullishEngulfing = equities[i].date[d];
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

					int stop = 0;

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
							float addProfitToDate = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
							if (isnan(addProfitToDate))
								addProfitToDate = 0.0;

							foundPatterns[f].tickerAndDate.push_back(equities[i].ticker + ":" + equities[i].date[d] + ":" + to_string(addProfitToDate));
							foundPatterns[f].numberOfOccurences++;
							if (givenDay > 0) {
								if (profitUpToGivenDay == false) {
									if (((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay]) > highestProfitabilityOfGivenDay)
										highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];

									foundPatterns[f].profitabilityOfGivenDay += (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
									if ((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay] > 0.0) {
										foundPatterns[f].greenDay += 1.0;
									}
									else {
										foundPatterns[f].redDay += 1.0;
									}
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

						float addProfitToDate = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
						if (isnan(addProfitToDate))
							addProfitToDate = 0.0;

						c.tickerAndDate.push_back(equities[i].ticker + ":" + equities[i].date[d] + ":" + to_string(addProfitToDate));
						c.candleStickType = extractedPattern;
						c.numberOfOccurences = 1;
						if (givenDay > 0) {
							if (profitUpToGivenDay == false) {
								if (((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay]) > highestProfitabilityOfGivenDay)
									highestProfitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];

								c.profitabilityOfGivenDay = (equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay];
								if ((equities[i].close[d + givenDay] - equities[i].open[d + givenDay]) / equities[i].open[d + givenDay] > 0.0) {
									c.greenDay += 1.0;
								}
								else {
									c.redDay += 1.0;
								}
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

		if (foundPatterns[p].numberOfOccurences == 1) {
			float profit = (foundPatterns[p].profitabilityOfGivenDay) * (foundPatterns[p].greenDay / foundPatterns[p].redDay);
			foundPatterns[p].measuredProfit = profit;
			continue;
		}

		if (foundPatterns[p].numberOfOccurences < 30)
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

		if (foundPatterns[p].greenDay / foundPatterns[p].redDay < 1.0)
			continue;

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

	return topPatterns;
}

int main() {

	//create a slider
	// low number of occurrences -------       number of occurrences       ---------- max number

	//rsi will affect G/R ratio: if rsi is 40 or below, on a certain range, is the next day green or red?
	//rsi might affect how much profitability 

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

	vector<CandleStickPattern> top100BullishEngulfing = calculateHighestProfitability(2, 1, false, 100, "bullish engulfing", 9);
	
	for (int i = 0; i < top100BullishEngulfing.size(); i++) {
		cout << top100BullishEngulfing[i].candleStickType << endl;
		cout << "occurences: " + to_string(top100BullishEngulfing[i].numberOfOccurences) + ", average profit: " + 
			to_string(top100BullishEngulfing[i].profitabilityOfGivenDay) + ", totalProfit: " + 
			to_string(top100BullishEngulfing[i].measuredProfit)
			+ ", G/R ratio: " + to_string(top100BullishEngulfing[i].greenDay / top100BullishEngulfing[i].redDay) << endl;
	}
	
	if (top100BullishEngulfing.size() > 0) {
		for (int j = 0; j < top100BullishEngulfing[0].tickerAndDate.size(); j++) {
			cout << top100BullishEngulfing[0].tickerAndDate[j] << endl;
		}
	}

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