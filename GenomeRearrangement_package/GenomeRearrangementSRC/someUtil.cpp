// $Id: someUtil.cpp 15479 2016-10-10 16:25:21Z elilevy $

#include "someUtil.h"
#include "errorMsg.h"
#include "RandomGenerators.h"
#include <time.h>
#include <cmath>
#include <ctime>
#include <chrono>
#include <iterator>
#include <algorithm>
#include <string>
#include <cctype>
#include <cassert>
#include <iomanip>
using namespace std;

//disabled by Tal Pupko on 22.6.2017 as it causes compilation probems.
// for the _mkdir call
//#if defined(WIN32) || defined(SunOS) || defined(solaris)
//  #include <direct.h>
//#else
 // #include <sys/file.h>
//  #include <dirent.h>
//  #include <io.h>
//#endif

//swap between the 4 variables such that the first becomes the second, second becomes the third and third becomes the fourth.
//used in functoin mnbrack below.
void shift3(MDOUBLE &a, MDOUBLE &b, MDOUBLE &c, const MDOUBLE d) {
	a=b;
	b=c;
	c=d;
}

MDOUBLE computeAverage(const vector<int>& vec) {
	MDOUBLE sum=0.0;
	for (size_t i=0; i < vec.size(); ++i) {
		sum+=static_cast<MDOUBLE>(vec[i]);
	}
	return sum/static_cast<MDOUBLE>(vec.size());
}

// X ~ Poisson(lamda) -->	P(X=k) = ((lamda^k)/k!) * e^(-lamda)
// It isn't smart to first calculate factorial(k) because the size of long int limits this calculation to k<=13
MDOUBLE copmutePoissonProbability(const int& k, const long double& lamda)
{
	assert(k>=0);
	long double tmp = pow(lamda,k); // tmp = (lamda^k)/k!
	
	for (int i=2; i<=k; ++i)
		tmp/=i;

	return (tmp * exp(-lamda));
}


MDOUBLE computeAverage(const vector<MDOUBLE>& vec, const Vdouble*  weightsV) {
	MDOUBLE sum=0.0;
	if(weightsV && !(weightsV->size() == vec.size() ))
		errorMsg::reportError("Using computeAverage with weights, where the number of weights not equal values");
	for (size_t i=0; i < vec.size(); ++i){
		if(weightsV)
			sum+=vec[i]* (*weightsV)[i];
		else
			sum+=vec[i];
	}
	return sum/static_cast<MDOUBLE>(vec.size());
}

MDOUBLE computeAverageOfAbs(const vector<MDOUBLE>& vec, const Vdouble*  weightsV) {
	MDOUBLE sum=0.0;
	if(weightsV && !(weightsV->size() == vec.size() ))
		errorMsg::reportError("Using computeAverage with weights, where the number of weights not equal values");
	for (size_t i=0; i < vec.size(); ++i){
		if(weightsV)
			sum+=abs(vec[i]* (*weightsV)[i]);
		else
			sum+=abs(vec[i]);
	}
	return sum/static_cast<MDOUBLE>(vec.size());
}

MDOUBLE computeMedian(const vector<MDOUBLE>& vec) {
	size_t vecSize = vec.size();
	if (vecSize<1)
		return 0;
	vector< vecElem<MDOUBLE> > sortVec(vecSize);
	for (size_t x =0; x < vecSize ; ++x)
	{
		sortVec[x].setValue(vec[x]);
		sortVec[x].setPlace(x);
	}
	sort(sortVec.begin(), sortVec.end());
	sort(sortVec.begin(), sortVec.end());//A.M. why twice?

	int highMedianIndex;
	if(vecSize>1)
		highMedianIndex = int((vecSize+1)/2);
	else
		highMedianIndex = int((vecSize)/2); // thus, highMedianIndex==0

	
	MDOUBLE	median = sortVec[highMedianIndex].getValue();
	return median;
}

//// if quantile=0.5, the median is returned, if quantile=0.1, the low-ton-percentile is returned, quantile=0.9, the top-90-percentile is returned
MDOUBLE computeQuantileFrac(const vector<MDOUBLE>& vec, MDOUBLE quantile) {
	size_t vecSize = vec.size();
	vector< vecElem<MDOUBLE> > sortVec(vecSize);
	for (size_t x =0; x < vecSize ; ++x)
	{
		sortVec[x].setValue(vec[x]);//A.M. this is similar to the median and can be transferred to a function
		sortVec[x].setPlace(x);
	}
	sort(sortVec.begin(), sortVec.end());
	sort(sortVec.begin(), sortVec.end());

	int qIndex = int((vecSize+1)*quantile);
	MDOUBLE quantileVal = sortVec[qIndex].getValue();
	return quantileVal;
}

//// if quantile=2, the median is returned, if quantile=10, the ten-percentile is returned
MDOUBLE computeQuantile(const vector<MDOUBLE>& vec, MDOUBLE quantile) {
	MDOUBLE dividerForRank = 1+ 1.0/(quantile-1);

	size_t vecSize = vec.size();
	vector< vecElem<MDOUBLE> > sortVec(vecSize);
	for (size_t x =0; x < vecSize ; ++x)
	{
		sortVec[x].setValue(vec[x]); //A.M. same here. the code lacks in modularity
		sortVec[x].setPlace(x);
	}
	sort(sortVec.begin(), sortVec.end());
	sort(sortVec.begin(), sortVec.end());

	int qIndex = int((vecSize+1)/dividerForRank);
	MDOUBLE quantileVal = sortVec[qIndex].getValue();
	return quantileVal;
}


MDOUBLE computeStd(const vector<int>& vec) {// page 60, Sokal and Rohlf
	MDOUBLE sum=0.0;
	MDOUBLE sumSqr=0.0;
	MDOUBLE vecSize = static_cast<MDOUBLE>(vec.size());
	for (size_t i=0; i < vec.size(); ++i) {
		sum+=static_cast<MDOUBLE>(vec[i]);
		sumSqr+=(static_cast<MDOUBLE>(vec[i])*static_cast<MDOUBLE>(vec[i]));
	}
	MDOUBLE res= sumSqr-(sum*sum/vecSize); 
	res /= (vecSize-1.0);
	res = sqrt(res);
	return res;
}

MDOUBLE computeStd(const vector<MDOUBLE>& vec) {// page 60, Sokal and Rohlf
	MDOUBLE sum=0.0;
	MDOUBLE sumSqr=0.0;
	MDOUBLE vecSize = static_cast<MDOUBLE>(vec.size());
	for (size_t i=0; i < vec.size(); ++i) {
		sum+=vec[i];
		sumSqr+=(vec[i]*vec[i]);
	}
	MDOUBLE res= sumSqr-(sum*sum/vecSize);
	res /= (vecSize-1.0);
	res = sqrt(res);
	return res;
}

void computeRelativeFreqsFollowingOneChanged(MDOUBLE  newValFreq, int indexNewFreq,Vdouble &freqs){
	//A.M. not so clear. need documentation
	MDOUBLE proportionAfterOptimization = 1.0 - newValFreq;
	MDOUBLE proportionBeforeOptimization = 1.0 - freqs[indexNewFreq];
	MDOUBLE sum = 0.0;
	for (size_t i=0; i<freqs.size(); ++i) {
		if (i==indexNewFreq){
			freqs[i] = newValFreq;
		}
		else {
			freqs[i] = proportionAfterOptimization*freqs[i]/proportionBeforeOptimization;
		}
		sum+=freqs[i];
	}
	if (!DEQUAL(sum,1.0)) {
		errorMsg::reportError("Error in computeRelativeFreqsFollowingOneChanged, sum not equal to 1");
	}
}


char mytolower(char in){return tolower(in);}
char mytoupper(char in){return toupper(in);}

void toLower(string& str) {
	transform (str.begin(), str.end(), str.begin(), mytolower);
}
void toUpper(string& str) {
	transform (str.begin(), str.end(), str.begin(), mytoupper);
}
string toUpper2(const string& str)
{
	string res("");
	transform (str.begin(), str.end(), res.begin(), mytoupper);
	return res;
}

bool allowCharSet(const string& allowableChars, const string& string2check) {
// this function check if all the character in string2check are made of characters from allowableChars
	for (size_t i=0; i < string2check.size(); ++i) {
		// now checking for string2check[i]
		size_t j;
		for (j=0; j < allowableChars.size(); ++j) {
			if (string2check[i] == allowableChars[j]) {
				break;
			}
		}
		if (j==allowableChars.size()) return false;
	}
	return true;
}

bool isCharInString(const string& stringToCheck, const char charToCheck) {
	for (size_t i=0; i < stringToCheck.size(); ++i ) {
		if (stringToCheck[i] == charToCheck) return true;
	}
	return false;
}

string double2string(const double x, const size_t lenght, bool round){
	
	// first getting the integer part:
	double x_abs = fabs(x);
	int theIntegerPart = static_cast<int>(x_abs);
	double theRemainingPart = fabs(x_abs-theIntegerPart);
	int integerRepresentingTheRemainingPart = static_cast<int>(theRemainingPart*pow(10.0,lenght));
	if (round) {
		integerRepresentingTheRemainingPart = static_cast<int>(theRemainingPart*pow(10.0,lenght)+0.5);
		if (integerRepresentingTheRemainingPart == pow(10.0,lenght)) {
			integerRepresentingTheRemainingPart = 0;
			theIntegerPart++;
		}
	}

	string part1 = int2string(theIntegerPart);
	string part2 = int2string(integerRepresentingTheRemainingPart);
	while (part2.length()<lenght){
		part2.insert(0, "0");
	}

	string result("");
	if (x < 0.0)
		result += "-";
	result += part1;
	result += ".";
	result += part2;

	// removing 0 from the end
	size_t i = result.length()-1;
	while (result[i]!='.' && i>0 && result[i]=='0'){
		result.erase(i);
		i--;
	}
	
	// removing "." if this is the last character in the string.
	if (result[result.length()-1]=='.')
	result.erase(result.length()-1);

	return result;
}

string int2string(const int num) {
// the input to this program is say 56
// the output is the string "56"
// this version of int2string is more portable 
// than sprintf like functions from c;
// or sstream of stl.
	if (num == 0) return "0";
	string res;
	int i = abs(num);
	
	
	int leftover;
	char k;
	while (i) {
		leftover = i%10;
		k = '0'+leftover;
		res = k+res;
		i/=10;
	}
	if (num<0) res = "-" + res;
	return res;
};
/*
void printTime(ostream& out) {
	time_t ltime;
	time( &ltime );
	const size_t SIZE = 30;
	//char time_str[SIZE];
	//	out<<"# the date is "<< ctime( &ltime )<<endl;
	#ifdef WIN32
		char time_str[SIZE];
		ctime_s(time_str, SIZE, &ltime);
		out << "# the date is " << time_str << endl;
	#else
		const time_t time_str[SIZE];
		ctime_r(time_str, SIZE, &ltime);
		out << "# the date is " << time_str << endl;
	#endif
	//out << "# the date is " << time_str << endl;
}
*/
MDOUBLE string2double(const string& inString) {

	if (allowCharSet("0123456789.eE+-",inString) == false) {
		errorMsg::reportError(" error in function string2double ");
	}
	
	// first decide if the format is like 0.00343 (regularFormat) or
	// if it is in the form of 0.34e-006 for example

	bool regularFormat = true;
	size_t i;
	for (i=0; i < inString.size(); ++i) {
		//A.M. why not: if (allowCharSet("0123456789.+-",inString) == false) regularFormat = false;
		if ((inString[i] == 'e' ) || (inString[i] == 'E' )) {
			regularFormat = false; 
			break;
		}
	}

	if (regularFormat) {
			MDOUBLE dDistance = atof(inString.c_str());
			return dDistance;
	}
	else {
		string b4TheExp;
		bool plusAfterTheExp = true;
		string afterTheExp;

		// b4 the exp
		for (i=0; i < inString.size(); ++i) {
			if ((inString[i] != 'e' ) && (inString[i] != 'E' )){
				b4TheExp += inString[i];
			}
			else break;
		}
		++i; //now standing after the exp;
		if (inString[i] == '-' ) {
			plusAfterTheExp = false;
			++i;
		}
		else if (inString[i] == '+' ) {
			plusAfterTheExp = true;
			++i;
		}
		else plusAfterTheExp = true; // the number is like 0.34e43

		for (; i < inString.size(); ++i) {
			afterTheExp += inString[i];
		}

		MDOUBLE res = 0.0;
		MDOUBLE dDistance = atof(b4TheExp.c_str());
		int exponentialFactor = atoi(afterTheExp.c_str());
		if (plusAfterTheExp) res = dDistance * pow(10.0,exponentialFactor);
		else res = dDistance * pow(10.0,-exponentialFactor);
		return res;
	}

	
}


bool checkThatFileExist(const string& fileName) {
	ifstream file1(fileName.c_str());
	if (!file1.good()) return false;
	file1.close();
	return true;
}

void putFileIntoVectorStringArray(istream &infile,vector<string> &inseqFile){
	inseqFile.clear();
	string tmp1;
	while (getline(infile,tmp1, '\n' ) ) {
		if (tmp1.empty()) continue;
		if (tmp1.size() > 100000) { // was 15000 
			vector<string> err;
			err.push_back("Unable to read file. It is required that each line is no longer than");
			err.push_back("15000 characters. "); //A.M. size was changed to 100000 but error message wasn't updated
			errorMsg::reportError(err,1); 
		}
		if (tmp1[tmp1.size()-1]=='\r') {// in case we are reading a dos file 
			tmp1.erase(tmp1.size()-1);
		}// remove the traling carrige-return
		inseqFile.push_back(tmp1);
	}
}

bool fromStringIterToInt(string::const_iterator & it, // ref must be here
						const string::const_iterator endOfString,
						int& res) {// the ref is so that we can use the it after the func.
	while (it != endOfString) {
		if ((*it == ' ') || (*it == '\t')) ++it;else break; // skipping white spaces.
	}
	if (it != endOfString) {
		if (isdigit(*it) || (*it == '-') || (*it == '+')){
			int k = atoi(&*it);
			if ((*it == '-') || (*it == '+'))  ++it;
			for (int numDig = abs(k); numDig>0; numDig/=10) ++it;
			res = k;
			return true;
		}
		else return false; //unable to read int From String
	}
	return false; //unable to read int From String
	
}

bool fromStringIterToSize_t(string::const_iterator & it, // ref must be here
	const string::const_iterator endOfString,
	size_t& res) {// the ref is so that we can use the it after the func.
	while (it != endOfString) {
		if ((*it == ' ') || (*it == '\t')) ++it; else break; // skipping white spaces.
	}
	if (it != endOfString) {
		if (isdigit(*it) || (*it == '+')) {
			int k = atoi(&*it);
			if (*it == '+') ++it;
			for (int numDig = abs(k); numDig>0; numDig /= 10) ++it;
			res = k;
			return true;
		}
		else return false; //unable to read size_t From String
	}
	return false; //unable to read size_t From String

}

string* searchStringInFile(const string& string2find,
						   const int index,
						   const string& inFileName) {
	ifstream f;
	f.open(inFileName.c_str());
	if (!f.good()) {
		string tmp = "Unable to open file name: "+inFileName+" in function searchStringInFile"; 
		errorMsg::reportError(tmp);
	}

	string numm = int2string(index);
	string realString2find = string2find+numm;

	istream_iterator<string> is_string(f);
	istream_iterator<string> end_of_stream;

	is_string = find(is_string,end_of_stream,realString2find);
	if(is_string == end_of_stream) {f.close();return NULL;}
	else {
		is_string++;
		if(is_string == end_of_stream) {f.close();return NULL;};
		string* s = new string(*is_string);
		f.close();
		return s;
	}
	f.close();
	return NULL;
}
string* searchStringInFile(const string& string2find,
						   const string& inFileName) {// return the string that is AFTER the string to search.
	ifstream f;
	f.open(inFileName.c_str());
	if (!f.good()) {
		string tmp = "Unable to open file name: "+inFileName+" in function searchStringInFile"; 
		errorMsg::reportError(tmp);
	}
	string realString2find = string2find;

	istream_iterator<string> is_string(f);
	istream_iterator<string> end_of_stream;

	is_string = find(is_string,end_of_stream,realString2find);
	if(is_string == end_of_stream) {f.close();return NULL;}
	else {
		is_string++;
		if(is_string == end_of_stream) {f.close();return NULL;};
		string* s = new string(*is_string);
		f.close();
		return s;
	}
	f.close();
	return NULL;
}
bool doesWordExistInFile(const string& string2find,const string& inFileName) {
	// A.M. why do we need this? we can use the previous one and compare to NULL. too much redundency
	ifstream f;
	f.open(inFileName.c_str());
	if (!f.good()) {
		string tmp = "Unable to open file name: "+inFileName+" in function searchStringInFile"; 
		errorMsg::reportError(tmp);
	}

	istream_iterator<string> is_string(f);
	istream_iterator<string> end_of_stream;

	is_string = find(is_string,end_of_stream,string2find);
	if(is_string == end_of_stream) return false;
	else return true;
}

string takeCharOutOfString(const string& charsToTakeOut, const string& fromString) {
	string finalString;
	for (size_t i=0; i<fromString.size(); ++i) {
		bool goodChar = true;
		for (size_t j=0; j < charsToTakeOut.size(); ++j) {
			if (fromString[i]== charsToTakeOut[j]) goodChar = false;
		}
		if (goodChar) finalString+=fromString[i];
	}
	return finalString;
}

bool DEQUAL(const MDOUBLE x1, const MDOUBLE x2, MDOUBLE epsilon/*1.192092896e-07F*/) {
	return (fabs(x1-x2)<epsilon);
}

bool DBIG_EQUAL(const MDOUBLE x1, const MDOUBLE x2, MDOUBLE epsilon/*1.192092896e-07F*/){
	return ((x1 > x2) || DEQUAL(x1, x2,epsilon));
}


bool DSMALL_EQUAL(const MDOUBLE x1, const MDOUBLE x2, MDOUBLE epsilon/*1.192092896e-07F*/){ 
	return ((x1 < x2) || DEQUAL(x1, x2,epsilon));
}

/* // disabled by TP as this causes compilation problems.
void createDir(const string & curDir, const string & dirName){// COPYRIGHT OF ITAY MAYROSE.
	string newDir;
	if (curDir == "")
		newDir = dirName;
	else
		newDir = curDir + string("/") + dirName;
#ifdef WIN32
	if( _mkdir(newDir.c_str()) == 0 ){
		LOG(5, << "Directory " <<newDir<<" was successfully created\n"<<endl);
    }else{
		if (errno == EEXIST) {
			LOG(5,<<"Directory already exist\n");
			return;
		} else {
		string err = "Problem creating directory " + newDir + " \n";
		LOG(5, << err << endl);
		errorMsg::reportError(err);
		}
	}
#else
	DIR * directory = opendir(newDir.c_str());
	if (directory == NULL) {
		string sysCall = "mkdir " + newDir;
        	system(sysCall.c_str());
	}
	else{
		string err = "Directory " + newDir + " already exists  \n";
		LOG(5, << err << endl);
		//errorMsg::reportError(err);
		
	}
#endif
}
*/
//scale vecToScale so that its new average is AvgIn. return the scaling factor. 
MDOUBLE scaleVec(Vdouble& vecToScale, const MDOUBLE avgIn)
{
	size_t vecSize = vecToScale.size();
	MDOUBLE sum = 0;
	for (size_t x = 0; x<vecSize; ++x)
	{
		sum += vecToScale[x];
	}
	MDOUBLE avg = sum/vecSize;
	MDOUBLE scaleFactor = avgIn / avg;
	
	for (size_t i = 0; i<vecSize; ++i)
	{
		vecToScale[i] *= scaleFactor;
	}

	MDOUBLE newAvg = computeAverage(vecToScale);
	if (fabs(newAvg - avgIn) > 0.001)
		errorMsg::reportError(" problem - scalled average is not avgIn after scalling!!!");
	return scaleFactor;
}

//calculates the mean square error distance between 2 vectors:
MDOUBLE calcMSEDistBetweenVectors(const Vdouble& oneRatesVec, const Vdouble& otherRatesVec)
{
	MDOUBLE res = 0.0;
	if (oneRatesVec.size() != otherRatesVec.size())
		errorMsg::reportError("the two vectors to be compared are not the same size in function SimulateRates::calcDistBetweenRatesVectors()");
	
	for (size_t i=0; i<oneRatesVec.size(); ++i)
	{
		MDOUBLE diff = oneRatesVec[i] - otherRatesVec[i];
		res += diff * diff;
	}

	res /= oneRatesVec.size();
	return res;
}

//calculates the mean absolute deviations distance between 2 vectors:
MDOUBLE calcMADDistBetweenVectors(const Vdouble& oneRatesVec, const Vdouble& otherRatesVec)
{
	MDOUBLE res = 0.0;
	if (oneRatesVec.size() != otherRatesVec.size())
		errorMsg::reportError("the two vectors to be compared are not the same size in function SimulateRates::calcDistBetweenRatesVectors()");
	
	for (size_t i=0; i<oneRatesVec.size(); ++i)
	{
		MDOUBLE diff = oneRatesVec[i] - otherRatesVec[i];
		res += fabs(diff);
	}

	res /= oneRatesVec.size();
	return res;
}

MDOUBLE calcRelativeMADDistBetweenVectors(const Vdouble& trueValues, const Vdouble& inferredValues, const MDOUBLE threshhold/*0.0*/)
{
	//A.M. need documentation. not trivial to understand. Bad var naming
	MDOUBLE res = 0.0;
	if (inferredValues.size() != trueValues.size())
		errorMsg::reportError("the two vectors to be compared are not the same size in function SimulateRates::calcDistBetweenRatesVectors()");
	
	int counter = 0;
	for (size_t i=0; i<inferredValues.size(); ++i)
	{
		if (trueValues[i] < threshhold)
			continue;
		MDOUBLE diff = fabs(inferredValues[i] - trueValues[i]);
		res += (diff / trueValues[i]);
		++counter;
	}

	res /= counter;
	return res;
}

//calculates the relative mean square error distance between 2 vectors:
//The difference from a regualar MSE is that for each position the squared difference is devided by the true value
//if threshhold > 0: if trueValues[i] < threshhold then do not add the rse for this psition to the result
MDOUBLE calcRelativeMSEDistBetweenVectors(const Vdouble& trueValues, const Vdouble& inferredValues, const MDOUBLE threshhold/*0.0*/ )
{
	MDOUBLE res = 0.0;
	if (inferredValues.size() != trueValues.size())
		errorMsg::reportError("the two vectors to be compared are not the same size in function SimulateRates::calcDistBetweenRatesVectors()");
	
	int counter = 0;
	for (size_t i=0; i<inferredValues.size(); ++i)
	{
		if (trueValues[i] < threshhold)
			continue;
		MDOUBLE diff = inferredValues[i] - trueValues[i];
		res += diff * diff / trueValues[i];
		++counter;
	}

	res /= counter;
	return res;
}


MDOUBLE calcRankCorrelation(const Vdouble& oneRatesVec, const Vdouble& otherRatesVec)
{	
	MDOUBLE res = 0.0;
	Vdouble orderVec1, orderVec2;
	MDOUBLE s_one = orderVec(oneRatesVec, orderVec1);
	MDOUBLE s_two = orderVec(otherRatesVec, orderVec2); //A.M. how do you know that they'll be ordered the same?
	size_t seqLength = oneRatesVec.size();

	MDOUBLE diff, sum_diff_sqr = 0;
	for (size_t i=0; i<seqLength; ++i)
	{
		diff = orderVec1[i] - orderVec2[i];
		sum_diff_sqr += pow(diff, 2);
	}

	MDOUBLE en3n = (seqLength * (pow(static_cast<double>(seqLength), 2.0) -1)); //n^3 -n
	MDOUBLE numerator = 1.0 - ((6/en3n) * (sum_diff_sqr + (s_one + s_two)/12.0));
	MDOUBLE denum = sqrt((1.0 - s_one/en3n) * (1.0 - s_two/en3n));
	res = numerator/ denum;
	return res;
}

/********************************************************************************************
calculates the spearman rank correlation value, Ofir implementation
*********************************************************************************************/
MDOUBLE calcRankCorrelation2(const Vdouble& oneRatesVec, const Vdouble& otherRatesVec)
{
	size_t vecLen = oneRatesVec.size();
	if(vecLen != otherRatesVec.size())
		errorMsg::reportError("calcRankCorrelation2. Vectors length differ");	
	
	Vdouble orderVec1, orderVec2;
	orderRankNoTies(oneRatesVec, orderVec1);
	orderRankNoTies(otherRatesVec, orderVec2);
	
	MDOUBLE n = (double)vecLen;
	MDOUBLE dif,r,sum_dif=0;
	for (size_t i=0; i<vecLen; ++i)
	{
		dif = orderVec1[i] - orderVec2[i];
		sum_dif += pow(dif, 2);
	}
	r=1-(6*sum_dif)/(n*(pow (n,2)-1));
	return r;
}


/********************************************************************************************
Pearson's correlation co-efficient
*********************************************************************************************/
MDOUBLE calcPearsonCorrelation(const Vdouble& oneRatesVec, const Vdouble& otherRatesVec, const int numberOfSignificantDigits)
{
	MDOUBLE res = 0.0;
	
	//MDOUBLE cov = calcCoVariance(oneRatesVec, otherRatesVec);
	//MDOUBLE sdOne = computeStd(oneRatesVec);
	//MDOUBLE sdOther = computeStd(otherRatesVec);
	//res = cov/(sdOne*sdOther);

	size_t seqLength = oneRatesVec.size();
	MDOUBLE meanOne = computeAverage(oneRatesVec);
	MDOUBLE meanOther = computeAverage(otherRatesVec);
	MDOUBLE cov = 0;
	MDOUBLE sdOne = 0;
	MDOUBLE sdOther = 0;
	for (size_t i=0; i<seqLength; ++i)
	{
		cov+=(oneRatesVec[i]-meanOne)*(otherRatesVec[i]-meanOther);	// crossProductSum  
		sdOne+=(oneRatesVec[i]-meanOne)*(oneRatesVec[i]-meanOne);		// sqDevX
		sdOther+=(otherRatesVec[i]-meanOther)*(otherRatesVec[i]-meanOther); //  sqDevY
	}
	res = cov/ (sqrt(sdOne)*sqrt(sdOther)); // no need to divide by seqLength -1, since canceled out
	
	MDOUBLE rounded = static_cast<double>(floorf(static_cast<float>(res * pow(10.0,numberOfSignificantDigits) + 0.5)) / pow(10.0,numberOfSignificantDigits)); // if not rounded, perfect correlations may return 1.000002, for example
	return rounded;
}

/********************************************************************************************
Benjamini�Hochberg�Yekutieli procedure controls the false discovery rate
*********************************************************************************************/
MDOUBLE computeFDRthreshold(Vdouble& pVals, MDOUBLE levelOfFDRcontroled, bool isPValsSorted){
	MDOUBLE FDRthreshold = 0;
	if(!isPValsSorted)
		sort(pVals.begin(),pVals.end());
	size_t indexAll = pVals.size();
	for (size_t i=0; i<pVals.size(); ++i){
		MDOUBLE correctedVal = (double)(i+1)/(double)indexAll *levelOfFDRcontroled;
		if( pVals[i] <= correctedVal){
			FDRthreshold = pVals[i];
		}			
	}
	return FDRthreshold;
}

MDOUBLE calcCoVariance(const Vdouble& oneRatesVec, const Vdouble& otherRatesVec)
{
	MDOUBLE cov = 0.0;
	MDOUBLE elemMulti = 0.0;
	size_t seqLength = oneRatesVec.size();
	MDOUBLE meanOne = computeAverage(oneRatesVec);
	MDOUBLE meanOther = computeAverage(otherRatesVec);
	for (size_t i=0; i<seqLength; ++i)
	{
		elemMulti += (oneRatesVec[i]-meanOne) * (otherRatesVec[i]-meanOther);
	}
	cov = elemMulti/(seqLength -1);
	return cov;
}


ostream &operator<<(ostream &out, const Vdouble &v){
  for (size_t j=0;j<v.size();++j)
    out<< v[j]<<" ";
  out <<endl;
  return(out);
}

ostream &operator<<(ostream &out, const VVdouble &m){
  for (size_t i=0;i<m.size();++i)
    out<<m[i];
  out <<endl;
  return(out);
}

void mult(Vdouble& vec, const MDOUBLE factor){
  for(size_t i=0;i<vec.size();++i)
    vec[i]*=factor;
}

void mult(VVdouble& vec, const MDOUBLE factor){
  for(size_t i=0;i<vec.size();++i)
    mult(vec[i],factor);
}



////orderVec - determine the relative order of vecIn
////returns orderVecOut[i] is the rank of vecIn[i]
////note that in case of ties the rank will be the midrank of the tied group
//Vdouble orderVec(const Vdouble& vecIn)
//{
//	int vecSize = vecIn.size();
//	Vdouble orderVecOut(vecSize);
//	vector< vecElem<MDOUBLE> > sortVec(vecSize);
//	for (int x =0; x < vecSize ; ++x)
//	{
//		sortVec[x].setValue(vecIn[x]);
//		sortVec[x].setPlace(x);
//	}
//	sort(sortVec.begin(), sortVec.end());
//	
//	//check for ties and correct their rank
//	Vdouble rankVec(vecSize);
//	MDOUBLE rank;
//	for (int i=0; i < vecSize; )
//	{
//		if (sortVec[i].getValue() != sortVec[i+1].getValue())
//		{//no tie
//			rankVec[i] = i;
//			++i;
//		}
//		else
//		{//tie
//			int to =0;
//			for (to = i+1; (to<=vecSize) && (sortVec[i].getValue() == sortVec[to].getValue());++to)
//				;//check how far the tie goes
//			to--;
//			rank = 0.5*(to + i); 
//			for (int ji = i; ji<= to; ji++)
//			{
//				rankVec[ji] = rank;
//			}
//
//			i = to+1;
//		}
//	}
//	for (int j =0; j < vecSize; ++j) {
//		assert ((rankVec[j] >= 0) && (rankVec[j] < vecSize));
//		orderVecOut[sortVec[j].getPlace()] = rankVec[j]; 
//	}
//	return orderVecOut;
//}

//orderVec - determine the relative order of vecIn
//orderVecOut[i] is the rank of vecIn[i]
//note that in case of ties the rank will be the midrank of the tied group
//return sum of n^3 - n where n is the number of elements in each tied group - see spearman rank correlation
MDOUBLE orderVec(const vector<MDOUBLE>& vecIn, vector<MDOUBLE>& orderVecOut)
{
	//A.M. not sure I understand this.
	size_t vecSize = vecIn.size();
	orderVecOut.resize(vecSize);
	vector< vecElem<MDOUBLE> > sortVec(vecSize);
	for (size_t x =0; x < vecSize ; ++x)
	{
		sortVec[x].setValue(vecIn[x]);
		sortVec[x].setPlace(x);
	}
	sort(sortVec.begin(), sortVec.end());
	//check for ties and correct their rank
	Vdouble rankVec(vecSize);
	MDOUBLE sumRankDif = 0; //sum(Fk^3 - Fk)

	MDOUBLE rank;
	for (size_t i=0; i < vecSize-1; ) // loop was till vecSize, out of range with sortVec[i+1]. Fixed (?)
	{
		if (sortVec[i].getValue() != sortVec[i+1].getValue())
		{//no tie
			rankVec[i] = i;
			++i;
		}
		else
		{//tie
			size_t to =0;
			for (to = i+1; (to<=vecSize) && (sortVec[i].getValue() == sortVec[to].getValue());++to)
				;//check how far the tie goes
			to--;
			rank = 0.5*(to + i); 
			for (size_t ji = i; ji<= to; ji++)
			{
				rankVec[ji] = rank;
			}

			int numTies = to - i +1; //number o fties in this group
			sumRankDif += numTies*numTies*numTies - numTies;
			i = to+1;
		}
	}
	
	for (size_t j =0; j < vecSize; ++j) {
	    assert ((rankVec[j] >= 0) && (rankVec[j] < vecSize));
	    orderVecOut[sortVec[j].getPlace()] = rankVec[j]; 
	}
	return sumRankDif;
}


void orderRankNoTies(const vector<MDOUBLE>& vecIn, vector<MDOUBLE>& orderVecOut)
{
	size_t vecSize = vecIn.size();
	orderVecOut.resize(vecSize);
	vector< vecElem<MDOUBLE> > sortVec(vecSize);
	for (size_t x =0; x < vecSize ; ++x)
	{
		sortVec[x].setValue(vecIn[x]);
		sortVec[x].setPlace(x);
	}
	sort(sortVec.begin(), sortVec.end());	
	for (size_t j =0; j < vecSize; ++j) {
	    orderVecOut[sortVec[j].getPlace()] = j; 
	}
	
}



void orderVec(const Vdouble& vecIn, vector< vecElem<MDOUBLE> >& orderVecOut)
{
	size_t vecSize = vecIn.size();
	orderVecOut.resize(vecSize);
	for (size_t x =0; x < vecSize ; ++x)
	{
		orderVecOut[x].setValue(vecIn[x]);
		orderVecOut[x].setPlace(x);
	}
	sort(orderVecOut.begin(), orderVecOut.end());
}


void splitString2(string str, string seperater, string &first, string &second)
{
     int i = (int)str.find(seperater); //find seperator
     if(i != -1)
     {
          int y = 0;
          if(!str.empty())
          {
               while(y != i)
               {
                    first += str[y++]; //creating first string
               }
               y = y+(int)seperater.length(); //jumping forward seperater length
               while(y != str.length())
               {
                    second += str[y++]; //creating second string
               }
               
          }
     }
     else
     {
          first = str;
          second = "NULL"; //if seperator is not there then second string == null 
     }
}


void splitString(const string& str,vector<string>& subStrs,const string& delimiter)
{
	// Skip delimiter at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiter,0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiter,lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a subStr, add it to the vector.
		subStrs.push_back(str.substr(lastPos,pos - lastPos));
		// Skip delimiter.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiter,pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiter,lastPos);
	}
}

Vint getVintFromStr(const string& inStr)
{
	Vint res;
	vector<string> outStr;
	splitString(inStr, outStr, ",");
	for (size_t i = 0; i < outStr.size(); ++i)
	{
		int x = atoi(outStr[i].c_str());
		res.push_back(x);
	}
	return res;
}

string getStrFromVint(const Vint& inVec)
{
	string res("");
	for (size_t i = 0; i < inVec.size(); ++i)
	{
		if (i > 0)
			res += ",";
		res += int2string(inVec[i]);
	}
	return res;
}


/********************************************************************************************
gainLoss project
*********************************************************************************************/
int fromIndex2gainIndex(const int i, const int gainCategories, const int lossCategories){
	//A.M. lacks documentation and better variable naming
	int gainIndex;
	if(lossCategories<=gainCategories){			
		gainIndex = (int)floor((double)i/(lossCategories) );
	}
	else{
		gainIndex = i%(gainCategories);			
	}
	return gainIndex;
}

int fromIndex2lossIndex(const int i, const int gainCategories, const int lossCategories){
	int lossIndex;
	//A.M. same as above
	if(lossCategories<=gainCategories){			
		lossIndex = i%(lossCategories);
	}
	else{
		lossIndex = (int)floor((double)i/(gainCategories) );		
	}
	return lossIndex;
}

int giveRandomState(const int alphabetSize, const int beginningState, const VVdouble &changeProbabilities) 
{
	//A.M. weird algorithm
	for (int loop = 0 ; loop < 100000 ; ++loop) 
	{
		MDOUBLE theRandNum = RandomGenerators::uniform();  // was talRandom::giveRandomNumberBetweenZeroAndEntry(1.0);
		MDOUBLE sum = 0.0;
		for (int state = 0; state < alphabetSize; ++state) 
		{
			sum += changeProbabilities[beginningState][state];
			if (theRandNum < sum) {
				return state;
			}
		}
	}
	errorMsg::reportError("giveRandomState: could not give random character. The reason is unknown.");
	return 1;

}

int giveRandomState(const int alphabetSize, const Vdouble &frequencies)  {
	for (int loop =0 ;loop<100000 ;loop++) {
		MDOUBLE theRandNum = RandomGenerators::uniform()*0.999; // was talRandom::giveRandomNumberBetweenZeroAndEntry(0.999);
		MDOUBLE sum = 0.0;
		for (int j=0; j < alphabetSize;++j) {
			sum+=frequencies[j];
			if (theRandNum<sum) return j;
		}
	} 
	errorMsg::reportError("giveRandomState: Could not give random character. The reason is probably that the frequencies do not sum to one.");
	return 1;
}
/********************************************************************************************
additional Math functions
*********************************************************************************************/
int sign(MDOUBLE r){
	if(r>0) return 1; 
	else return -1;
}
MDOUBLE factorial(int x) {
	MDOUBLE fac = 1;
	for (int i=2; i<=x; i++) 
		fac *= i;
	return fac;
}
MDOUBLE BinomialCoeff(int a, int b) {
	return factorial(a)/(factorial(b)*factorial(a-b));
}

MDOUBLE exponentResolver(Vdouble& valuesVec){
	//First find largest element in valuesVec
	MDOUBLE largest = VERYSMALL;
	int largestIndex = -1;
	for(size_t i = 0;i < valuesVec.size();++i){
		if(valuesVec[i] > largest){
			largest = valuesVec[i];
			largestIndex = i;
		}
	}
	if(largestIndex == -1){
		errorMsg::reportError("exponentResolver: Could not find the largest element in the input vector");
		return 1;
	}
	//Now sum over all elements that are greater than -50. Note that exp(-50) is way smaller than the famous EPSILON so we are pretty safe from neglecting anything significant
	MDOUBLE sum = 1.0; 
	MDOUBLE cutoff = -50;
	for(size_t i = 0;i < valuesVec.size();++i){
		if(i == largestIndex) continue;
		if((valuesVec[i]-largest) < cutoff) continue;
		sum += exp(valuesVec[i]-largest);
	}
	MDOUBLE result = largest+log(sum);
	return(result);
}

MDOUBLE sumVdouble(const Vdouble & vec){
	MDOUBLE sum = 0.0;
	for(size_t i = 0;i < vec.size();++i){
		sum += vec[i];
	}
	return(sum);
}
