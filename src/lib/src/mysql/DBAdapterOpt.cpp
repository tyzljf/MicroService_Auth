#include "DBAdapterOpt.h"

DBAdapterOpt::DBAdapterOpt()
{

}

DBAdapterOpt::~DBAdapterOpt()
{

}

void DBAdapterOpt::AddIntParam(std::vector<ParamToBind_t>& params, const uint64_t* intVal)
{
	params.push_back(ParamToBind_t());

	params[params.size() - 1].paramType = INT_PARAM;
	params[params.size() - 1].intVal = *intVal;
}

void DBAdapterOpt::AddShortParam(std::vector<ParamToBind_t>& params, const uint_t* intVal)
{
	params.push_back(ParamToBind_t());

	params[params.size() - 1].paramType = INT_PARAM;
	params[params.size() - 1].intVal = *intVal;
}

void DBAdapterOpt::AddPatternParam(std::vector<ParamToBind_t>& params, const string* strVal)
{
	params.push_back(ParamToBind_t());

	params[params.size() - 1].paramType = PATTERN_PARAM;
	params[params.size() - 1].strVal = "%" + *strVal + "%";
}

void DBAdapterOpt::AddBlobParam(std::vector<ParamToBind_t>& params, const string* strVal, uint64_t strSize)
{
	params.push_back(ParamToBind_t());

	params[params.size() - 1].paramType = BLOB_PARAM;
	params[params.size() - 1].strVal = *strVal;
	params[params.size() - 1].strSize = strSize;
}

void DBAdapterOpt::AddStringParam(std::vector<ParamToBind_t>& params, const string* strPtr)
{
	params.push_back(ParamToBind_t());

	params[params.size() - 1].paramType = STRING_PARAM;
	params[params.size() - 1].strVal = *strPtr;
}


void DBAdapterOpt::AddDefIntParam(std::vector<QueryResult_t>& params, uint64_t* intVal)
{
	params.push_back(QueryResult_t());

	if(intVal)
	{
		params[params.size() - 1].paramType = INT_PARAM;
		params[params.size() - 1].intVal = intVal;
	}
}

void DBAdapterOpt::AddDefShortParam(std::vector<QueryResult_t>& params, uint_t* intVal)
{
	params.push_back(QueryResult_t());

	if(intVal)
	{
		params[params.size() - 1].paramType = INT_PARAM;
		params[params.size() - 1].intVal = (uint64_t*)intVal;
	}
}

void DBAdapterOpt::AddDefPatternParam(std::vector<QueryResult_t>& params, string* strVal)
{
	params.push_back(QueryResult_t());

	if(strVal)
	{
		params[params.size() - 1].paramType = STRING_PARAM;
		params[params.size() - 1].strVal = strVal;
	}
}

void DBAdapterOpt::AddDefBlobParam(std::vector<QueryResult_t>& params, string* strVal, uint64_t strSize)
{
	params.push_back(QueryResult_t());

	if(strVal)
	{
		params[params.size() - 1].paramType = BLOB_PARAM;
		params[params.size() - 1].strVal = strVal;
		params[params.size() - 1].strSize = strSize;
	}
}

void DBAdapterOpt::AddDefStringParam(std::vector<QueryResult_t>& params, string* strVal)
{
	params.push_back(QueryResult_t());

	if(strVal)
	{
		params[params.size() - 1].paramType = STRING_PARAM;
		params[params.size() - 1].strVal = strVal;
	}
}

