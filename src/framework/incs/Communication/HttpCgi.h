class CHttpResponse
{
public:
	void SetHead(mp_string name, mp_string value);

private:
	map<mp_string, mp_string> m_headers;

};

