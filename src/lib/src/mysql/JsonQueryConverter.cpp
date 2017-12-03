#include "JsonQueryConverter.h"

JsonQueryConverter::JsonQueryConverter()
{

}

JsonQueryConverter::~JsonQueryConverter()
{

}

JSON_PARSE_E JsonQueryConverter::Init(DBAdapterOpt* adapterOpt)
{
	m_AdapterOpt = adapterOpt;


	return JSON_PARSE_OK;
}


