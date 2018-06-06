import getopt
import os
import sys
import csv
import operator

__author__ = 'Ben Loisch'

stocksDay1 = []
stocksDay2 = []
stocksDay3 = []
tickerDay1 = []
tickerDay2 = []
tickerDay3 = []
volumeMin = 0
volumeMax = 1000000000
rsiMin = 0
rsiMax = 100
marketCapMin = 0
marketCapMax = 1000000000000
volatilityMin = 0.0
volatilityMax = 1000.0
priceMin = 0.0
priceMax = 1000000.0
industry = -1
uniqueIndustries = []

class Stock:
    def __init__(self, data):
        self.ticker = data[0]
        self.volume = int(data[1])
        self.close = float(data[2])
        self.open = float(data[3])
        if (data[4] == ''):
            data[4] = 'nan'
        self.marketcap = float(data[4])
        self.industry = data[5]
        if self.industry not in uniqueIndustries:
            uniqueIndustries.append(self.industry)
        if (data[6] == ''):
            data[6] = 'nan'
        self.rsi = float(data[6])
        self.low = float(data[7])
        self.high = float(data[8])
        self.volatility = float(data[9])

def removeBadStocks():
    t = 0
    while t < (len(stocksDay1) - 1):
        if (not (tickerDay1[t] in tickerDay2)):
            stocksDay1.pop(t)
            tickerDay1.pop(t)
            t = t - 1
        t += 1
    t = 0
    while t < (len(stocksDay2) - 1):
        if (not (tickerDay2[t] in tickerDay3)):
            stocksDay2.pop(t)
            tickerDay2.pop(t)
            t = t - 1
        t += 1
    t = 0
    while t < (len(stocksDay3) - 1):
        if (not (tickerDay3[t] in tickerDay1)):
            stocksDay3.pop(t)
            tickerDay3.pop(t)
            t = t - 1
        t += 1
    t = 0
    while t < (len(stocksDay2) - 1):
        if (not (tickerDay2[t] in tickerDay1)):
            stocksDay2.pop(t)
            tickerDay2.pop(t)
            t = t - 1
        t += 1
    t = 0
    while t < (len(stocksDay1) - 1):
        if (not (tickerDay1[t] in tickerDay2)):
            stocksDay1.pop(t)
            tickerDay1.pop(t)
            t = t - 1
        t += 1


def bullishEngulfing():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close < stocksDay2[t].open):
            if (stocksDay3[t].close > stocksDay3[t].open):
                if (stocksDay3[t].low < stocksDay2[t].low and stocksDay3[t].open < stocksDay2[t].close):
                    if (stocksDay3[t].high > stocksDay2[t].high and stocksDay3[t].close > stocksDay2[t].open):
                        print(tickerDay3[t])

def bearishEngulfing():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close > stocksDay2[t].open):
            if (stocksDay3[t].close < stocksDay3[t].open):
                if (stocksDay3[t].high > stocksDay2[t].high and stocksDay3[t].open > stocksDay2[t].close):
                    if (stocksDay3[t].low < stocksDay2[t].low and stocksDay3[t].close < stocksDay2[t].open):
                        print(tickerDay3[t])

def oneWhiteSoldier():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close < stocksDay2[t].open):
            if (stocksDay3[t].close > stocksDay3[t].open):
                if (stocksDay3[t].open > stocksDay2[t].close and stocksDay3[t].open < stocksDay2[t].open):
                    if (stocksDay3[t].low > stocksDay2[t].close and stocksDay3[t].low < stocksDay2[t].open):
                        if (stocksDay3[t].close > stocksDay2[t].high):
                            print(tickerDay3[t])

def oneBlackCrow():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close > stocksDay2[t].open):
            if (stocksDay3[t].close < stocksDay3[t].open):
                if (stocksDay3[t].open < stocksDay2[t].close and stocksDay3[t].open > stocksDay2[t].open):
                    if (stocksDay3[t].high < stocksDay2[t].close and stocksDay3[t].high > stocksDay2[t].open):
                        if (stocksDay3[t].close < stocksDay2[t].low):
                            print(tickerDay3[t])

def bullishHarami():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close < stocksDay2[t].open):
            if (stocksDay3[t].close > stocksDay3[t].open):
                if (stocksDay3[t].high < stocksDay2[t].open and stocksDay3[t].low > stocksDay2[t].close):
                    print(tickerDay3[t])

def bearishHarami():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close > stocksDay2[t].open):
            if (stocksDay3[t].close < stocksDay3[t].open):
                if (stocksDay3[t].high < stocksDay2[t].close and stocksDay3[t].low > stocksDay2[t].open):
                    print(tickerDay3[t])

def bullishTweezer():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close < stocksDay2[t].open):
            if (stocksDay3[t].close > stocksDay3[t].open):
                if (max(stocksDay2[t].close, stocksDay2[t].low, stocksDay3[t].open, stocksDay3[t].low) - min(
                        stocksDay2[t].close, stocksDay2[t].low, stocksDay3[t].open, stocksDay3[t].low) <= 0.001 * stocksDay3[t].high):
                    print(tickerDay3[t])

def bearishTweezer():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay2[t].close > stocksDay2[t].open):
            if (stocksDay3[t].close < stocksDay3[t].open):
                if (max(stocksDay2[t].close, stocksDay2[t].high, stocksDay3[t].open, stocksDay3[t].high) - min(
                        stocksDay2[t].close, stocksDay2[t].high, stocksDay3[t].open, stocksDay3[t].high) <= 0.001 * stocksDay3[t].high):
                    print(tickerDay3[t])

def bullishMorningStar():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay1[t].close < stocksDay1[t].open):
            if (stocksDay2[t].close > stocksDay2[t].open):
                if (stocksDay3[t].close > stocksDay3[t].open):
                    if (stocksDay2[t].high < stocksDay1[t].low): #if day2 gaps down from day1
                        if (stocksDay3[t].low > stocksDay2[t].high): #if day3 gaps up from day2
                            print(tickerDay3[t])

def bearishEveningStar():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        if (stocksDay1[t].close > stocksDay1[t].open):
            if (stocksDay2[t].close < stocksDay2[t].open):
                if (stocksDay3[t].close < stocksDay3[t].open):
                    if (stocksDay2[t].low > stocksDay1[t].high): #if day2 gaps up from day1
                        if (stocksDay3[t].high < stocksDay2[t].low): #if day3 gaps down from day2
                            print(tickerDay3[t])

def simpleSearch():
    for t in range(0, len(stocksDay1)):

        if (stocksDay1[t].volume < volumeMin or stocksDay1[t].volume > volumeMax):
            continue
        if (stocksDay1[t].rsi < rsiMin or stocksDay1[t].rsi > rsiMax):
            continue
        if (stocksDay1[t].marketcap < marketCapMin or stocksDay1[t].marketcap > marketCapMax):
            continue
        if (stocksDay1[t].volatility < volatilityMin or stocksDay1[t].volatility > volatilityMax):
            continue
        if (stocksDay1[t].close < priceMin or stocksDay1[t].close > priceMax):
            continue
        if (industry != -1):
            if (stocksDay1[t].industry != uniqueIndustries[industry]):
                continue

        print(tickerDay3[t])

def printSectorRSI():
    mydict = {}

    for i in range(0, len(uniqueIndustries)):
        averageRSI = 0
        industryCounter = 0
        for t in range(0, len(stocksDay1)):
            if (stocksDay3[t].industry == uniqueIndustries[i]):
                averageRSI = averageRSI + stocksDay3[t].rsi
                industryCounter = industryCounter + 1

        mydict[uniqueIndustries[i]] = averageRSI / industryCounter

    sorted_x = sorted(mydict.items(), key=operator.itemgetter(1))
    for key, val in sorted_x:
        print(key + ", RSI: " + str(val))

def main(arg1='', arg2='', arg3=''):
    # check for valid file paths for arguments
    if len(arg1)==0:
        print("arg1 is not a valid file or full path\n")
        exit(0)
    if not os.path.isfile(arg1):
        print("arg1 is not a valid file or full path\n")
        exit(0)
    if len(arg2)==0:
        print("arg2 is not a valid file or full path\n")
        exit(0)
    if not os.path.isfile(arg2):
        print("arg2 is not a valid file or full path\n")
        exit(0)
    if len(arg3)==0:
        print("arg3 is not a valid file or full path\n")
        exit(0)
    if not os.path.isfile(arg3):
        print("arg3 is not a valid file or full path\n")
        exit(0)

    day1 = csv.reader(open(arg1, 'r'), delimiter=',')
    day2 = csv.reader(open(arg2, 'r'), delimiter=',')
    day3 = csv.reader(open(arg3, 'r'), delimiter=',')

    #read .csv files
    counter = 0
    for row in day1:
        if (counter > 0):
            stocksDay1.append(Stock(row))
            tickerDay1.append(row[0])
        counter = counter + 1
    counter = 0
    for row in day2:
        if (counter > 0):
            stocksDay2.append(Stock(row))
            tickerDay2.append(row[0])
        counter = counter + 1
    counter = 0
    for row in day3:
        if (counter > 0):
            stocksDay3.append(Stock(row))
            tickerDay3.append(row[0])
        counter = counter + 1

    removeBadStocks()

    while(True):

        print("\nSelect")
        print("0: Exit")
        print("1: Candlestick Help")
        print("s: Simple Search")
        print("2: Volume,RSI,MarketCap,Industry")
        print("3: Bullish Engulfing")
        print("3b: Bearish Engulfing")
        print("4: One White Soldier")
        print("4b: One Black Crow")
        print("5: Bullish Harami")
        print("5b: Bearish Harami")
        print("5h: Bullish Harami Help")
        print("6: Bullish Tweezer")
        print("6b: Bearish Tweezer")
        print("7: Bullish Morning Star")
        print("7b: Bearish Evening Star")
        print("8: Print Industry RSI")

        candlestickChoice = input()

        if (candlestickChoice == "0"):
            break
        elif (candlestickChoice == "1"):
            print("Upper wick represents selling pressure, bottom wick represents buying pressure")
        elif (candlestickChoice == "s"):
            simpleSearch()
        elif (candlestickChoice == "2"):
            candleStickVolumeMinChoice = input("Minimum Volume (default 0): ")
            if (candleStickVolumeMinChoice != ""):
                global volumeMin
                volumeMin = int(candleStickVolumeMinChoice)
            else:
                volumeMin = 0
            candleStickVolumeMaxChoice = input("Maximum Volume (default 1000000000): ")
            if (candleStickVolumeMaxChoice != ""):
                global volumeMax
                volumeMax = int(candleStickVolumeMaxChoice)
            else:
                volumeMax = 1000000000
            candleStickRsiChoiceMin = input("Minimum RSI (default 0): ")
            if (candleStickRsiChoiceMin != ""):
                global rsiMin
                rsiMin = int(candleStickRsiChoiceMin)
            else:
                rsiMin = 0
            candleStickRsiChoiceMax = input("Maximum RSI (default 100): ")
            if (candleStickRsiChoiceMax != ""):
                global rsiMax
                rsiMax = int(candleStickRsiChoiceMax)
            else:
                rsiMax = 100
            candleStickMarketCapMin = input("Minimum Market Cap (default 0): ")
            if (candleStickMarketCapMin != ""):
                global marketCapMin
                marketCapMin = int(candleStickMarketCapMin)
            else:
                marketCapMin = 0
            candleStickMarketCapMax = input("Maximum Market Cap (default 1000000000000): ")
            if (candleStickMarketCapMax != ""):
                global marketCapMax
                marketCapMax = int(candleStickMarketCapMax)
            else:
                marketCapMax = 1000000000000
            candleStickVolatilityMin = input("Minimum Volatility (default 0): ")
            if (candleStickVolatilityMin != ""):
                global volatilityMin
                volatilityMin = float(candleStickVolatilityMin)
            else:
                volatilityMin = 0
            candleStickVolatilityMax = input("Maximum Volatility (default 1000): ")
            if (candleStickVolatilityMax != ""):
                global volatilityMax
                volatilityMax = float(candleStickVolatilityMax)
            else:
                volatilityMax = 1000
            candlestickPriceMin = input("Minimum Price (default 0.0): ")
            if (candlestickPriceMin != ""):
                global priceMin
                priceMin = float(candlestickPriceMin)
            else:
                priceMin = 0
            candlestickPriceMax = input("Maximum Price (default 1000000.0): ")
            if (candlestickPriceMax != ""):
                global priceMax
                priceMax = float(candlestickPriceMax)
            else:
                priceMax = 1000000.0
            print("Pick an industry (default any)")
            for i in range(0, len(uniqueIndustries)):
                print(str(i) + ": " + uniqueIndustries[i])
            industryChoice = input()
            if (industryChoice != ""):
                global industry
                industry = int(industryChoice)
            else:
                industry = -1
        elif (candlestickChoice == "3"):
            bullishEngulfing()
        elif (candlestickChoice == "3b"):
            bearishEngulfing()
        elif (candlestickChoice == "4"):
            oneWhiteSoldier()
        elif (candlestickChoice == "4b"):
            oneBlackCrow()
        elif (candlestickChoice == "5"):
            bullishHarami()
        elif (candlestickChoice == "5b"):
            bearishHarami()
        elif (candlestickChoice == "5h"):
            print("Buy in on bullish harami when green day is just outside or at edge of boilinger bands")
        elif (candlestickChoice == "6"):
            bullishTweezer()
        elif (candlestickChoice == "6b"):
            bearishTweezer()
        elif (candlestickChoice == "7"):
            bullishMorningStar()
        elif (candlestickChoice == "7b"):
            bearishEveningStar()
        elif (candlestickChoice == "8"):
            printSectorRSI()

        input("Press Enter to Continue")

if __name__ == '__main__':
    main("C:\\Users\\benlo\\Desktop\\tradingview\\america_2018-05-23.csv",
         "C:\\Users\\benlo\\Desktop\\tradingview\\america_2018-05-24.csv",
         "C:\\Users\\benlo\\Desktop\\tradingview\\america_2018-06-06.csv")