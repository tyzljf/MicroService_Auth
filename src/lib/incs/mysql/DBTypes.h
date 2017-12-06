#ifndef __DB_TYPES_H
#define __DB_TYPES_H

#include <iostream>
#include <vector>
#include "Types.h"

enum PARAM_TYPE_E
{
	INT_PARAM,
	STRING_PARAM,
	PATTERN_PARAM,
	BLOB_PARAM
};

using namespace std;

#define MAX_STR_NAME	1024

class CommonFiled
{
public:
	CommonFiled()
	{
		paramType = INT_PARAM;        
		intPtr = NULL;        
		strPtr = NULL;        
		strSize = 0;
	}

	PARAM_TYPE_E getType() { return paramType; }
	uint64_t* getIntPtr() { return intPtr; } 
	string* getStrPtr() { return strPtr; }
	uint64_t getStrSize() { return strSize; }
	
	PARAM_TYPE_E paramType;
	uint64_t* intPtr;
	string* strPtr;
	uint64_t strSize;
};

class IntFiled : public CommonFiled
{
public:
	IntFiled(uint64_t* iptr)
	{  
		paramType = INT_PARAM;   
		intPtr = iptr;   
	}
};

class StrFiled : public CommonFiled
{
public:  
	StrFiled(string* sptr, uint64_t size) 
	{      
		paramType = STRING_PARAM;    
		strPtr = sptr;      
		strSize = size + 1; 
	}
};

class BlobFiled : public CommonFiled
{
public: 
	BlobFiled(string* sptr, uint64_t size)  
	{     
		paramType = BLOB_PARAM; 
		strPtr = sptr;       
		strSize = size + 1;  
	}
};

class PatternFiled : public CommonFiled
{
public: 
	PatternFiled(string* sptr, uint64_t size) 
	{
		paramType = PATTERN_PARAM;     
		strPtr = sptr;  
		strSize = size + 1;   
	}
};

class FiledRouter
{
public: 
	std::vector<CommonFiled> filed;  

	uint64_t getSize() { filed.size(); }  
	PARAM_TYPE_E getFiledType(uint64_t i) { return filed[i].getType(); } 
	uint64_t* getFiledIntPtr(uint64_t i) { return filed[i].getIntPtr(); }
	string* getFiledStrPtr(uint64_t i) { return filed[i].getStrPtr(); } 
	uint64_t getFiledSize(uint64_t i) { return filed[i].getStrSize(); }
};

class FlexQueryResult
{
public:
	PARAM_TYPE_E paramType;
	uint64_t* intPtr;
	char* strPtr; 
	uint64_t strSize;
};

#define SET_INT_FILED(name) \
	router.filed.push_back(IntFiled(&name));

#define SET_STRING_FILED(name, size)	\
	router.filed.push_back(StrFiled(&name, size));

#define SET_BLOB_FILED(name, size)	\
	router.filed.push_back(BlobFiled(&name, size));

#define SET_PATTERN_FILED(name, size) \
	router.filed.push_back(PatternFiled(&name, size));

#define START_FILED(type)       \
	type() {                    \
		init();                 \
	}                           \
	void init() {

#define END_FILED(type)      	\
	}                           \
	type& operator=(const type& rhs){ 		\
		if(this == &rhs)  return *this;    	\
		copy(rhs);                     	\
		return *this;                   	\
	}                                   	\
	type(const type& rhs){           		\
		init();                        		\
		copy(rhs);                      	\
	}                                   	\
	FiledRouter router;                 	\
	uint64_t getSize() { return router.getSize(); } \
	FiledRouter& getRouter() { return router;}

#endif
