void CResponseMsg::Send()
{
	if(m_contentType == CONTENT_JSON)
	{
		SendJson();
	}
	else if(m_contentType == CONTENT_ATTACHMENT)
	{
		SendAttachment();
	}
	else
	{
		//��ӡ��־��Ϣ
		m_httpResponse.Complete();
	}
}

void CResponseMsg::SendJson()
{
	if(m_RetCode != MP_SUCCESS)
	{
		
	}

}

void CResponseMsg::SendAttachment()
{
	

}

void CResponseMsg::PackageResponse()
{
	

}