#ifndef __DB_ADAPTEROPT_H
#define __DB_ADAPTEROPT_H

#include <vector>
#include <iostream>
#include "DBTypes.h"

using namespace std;

class ParamToBind_t
{
public:
	PARAM_TYPE_E paramType;
	uint64_t intVal;
	string strVal;
	uint_t strSize;
};


class QueryResult_t
{
public:
	PARAM_TYPE_E paramType;
	uint64_t *intVal;
	string *strVal;
	uint_t strSize;
};


class DBAdapterOpt
{
public:
	DBAdapterOpt();
	virtual ~DBAdapterOpt();

	void AddIntParam(std::vector<ParamToBind_t>& params, const uint64_t* intVal);
	void AddShortParam(std::vector<ParamToBind_t>& params, const uint_t* intVal);
	void AddPatternParam(std::vector<ParamToBind_t>& params, const string* strVal);
	void AddBlobParam(std::vector<ParamToBind_t>& params, const string* strVal, uint64_t strSize);
	void AddStringParam(std::vector<ParamToBind_t>& params, const string* strPtr);

	void AddDefIntParam(std::vector<QueryResult_t>& params, uint64_t* intVal);
	void AddDefShortParam(std::vector<QueryResult_t>& params, uint_t* intVal);
	void AddDefPatternParam(std::vector<QueryResult_t>& params, string* strVal);
	void AddDefBlobParam(std::vector<QueryResult_t>& params, string* strVal, uint64_t strSize);
	void AddDefStringParam(std::vector<QueryResult_t>& params, string* strPtr);
};


#endif

