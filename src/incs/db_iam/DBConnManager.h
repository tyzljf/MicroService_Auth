#ifndef __DB_CONNMANAGER_H
#define __DB_CONNMANAGER_H

#define MIN_DBCONN_NUM 1
#define MAX_DBCONN_NUM 100

class DBConn;

class DBConnManager
{
public:
	bool Init(std::size_t size, const std::string& dataSource);

	std::shared_ptr<DBConn>& GetConnection();
	
private:
	std::vector<std::shared_ptr<DBConn>> m_dbConnList;
	
};


#endif