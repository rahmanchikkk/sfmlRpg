//#include <vld.h>
#include "Game.h"
#include "ignore.h"
#include <libpq-fe.h>

int main(int argc, void** argv[]){
	// Program entry point.
	std::string s = "dbname=" + std::string(db_name) + "user=" + std::string(user) +  "password=" + std::string(password) + " host=localhost port=5432";
	const char* conninfo = s.c_str();
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		printf("Error while connecting to database server: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}
	printf("Connection established.\n");
	printf("Port: %s\n", PQport(conn));
	printf("Host: %s\n", PQhost(conn));
	printf("DB Name: %s\n", PQdb(conn));
	char* query = "SELECT * FROM person";
	PGresult* res = PQexec(conn, query);
	ExecStatusType status = PQresultStatus(res);
	printf("Query status: %s\n", PQresStatus(status));
	if (status != PGRES_TUPLES_OK) {
		printf("Error while executing query: %s\n", PQerrorMessage(conn));
		PQclear(res);
		PQfinish(conn);
		exit(1);
	}
	printf("Query executed successfully.\n");
	int rows = PQntuples(res);
	int cols = PQnfields(res);
	printf("Number of rows: %d\n", &rows);
	printf("Number of columns: %d\n", &cols);
	for (int i = 0; i < cols; ++i) {
		printf("%s\t", PQfname(res, i));
	}
	printf("\n");
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			printf("%s\t", PQgetvalue(res, i, j));
		}
		printf("\n");
	}
	PQclear(res);
	PQfinish(conn);
	{
		Game game;
		while(!game.GetWindow()->IsDone()){
			game.Update();
			game.Render();
			game.LateUpdate();
		}
	}
	return 0;
}