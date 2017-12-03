#ifndef __JSON_CONVERTER_H
#define __JSON_CONVERTER_H

#include "DBAdapterOpt.h"

enum JSON_PARSE_E
{
	JSON_PARSE_OK,
};

class JsonQueryConverter
{
public:
	JsonQueryConverter();
	virtual ~JsonQueryConverter();

public:
	JSON_PARSE_E Init(DBAdapterOpt *adapterOpt);

private:
	DBAdapterOpt* m_AdapterOpt;
};

#endif