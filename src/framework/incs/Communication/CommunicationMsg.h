#ifndef __COMMUNICATION_MSG_H
#define __COMMUNICATION_MSG_H

enum ContentType
{
	CONTENT_JSON,
	CONTENT_ATTACHMENT	
};

class CRequestMsg
{

};

class CResponseMsg
{
public:
	CResponseMsg(FCGX_Request* fcgxReq):m_httpResponse(fcgxReq)
	{
		m_contentType = CONTENT_JSON;

	}

	CResponseMsg()
	{
		m_contentType = CONTENT_ATTACHMENT;
		

	}

	void Send();
	void SendJson();
	void SendAttachment();
	void PackageResponse();

	Json::Value& GetJsonDataRef()
	{
		return m_jsonValueData;
	}

	Json::Value GetJsonData()
	{
		return m_jsonValueData;
	}

	void SetHttpStatus(mp_int httpSatus)
	{
		m_httpStatus = httpSatus;
	}

	void SetRetCode(mp_int retCode)
	{
		m_RetCode = retCode;
	}
	
private:
	ContentType m_contentType;
	mp_int 		m_httpStatus;
	mp_int 		m_RetCode;
	Json::Value m_jsonValueData;
	CHttpResponse m_httpResponse;
};

#endif
