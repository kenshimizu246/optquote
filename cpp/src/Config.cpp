
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filewritestream.h>

#include "Config.hpp"
#include "AppCtx.hpp"

using namespace std;
using namespace rapidjson;

namespace optquote {

Config::Config() {
	initialized = false;
}

Config::~Config() {
}

void Config::load(const char* filename){
	ifstream ifs(filename);
	string ss, str;
	Document doc;

	const char *cpidfile = "pidfile";
	const char *clogdir = "logdir";
	const char *cappdir = "appdir";
	const char *cport = "port";

	const char *cDbHost = "db_host";
	const char *cDbPort = "db_port";
	const char *cDbName = "db_name";
	const char *cDbUser = "db_user";
	const char *cDbPass = "db_pass";

	const char *cDaemon = "daemon";

	const char *cQuoteLivePeriod = "quoteLivePeriod";

	if (ifs.fail()) {
		throw runtime_error("file read failed!");
	}

	while (getline(ifs, ss)) {
		cout << "[" << ss << "]" << endl;
		str += ss;
	}
	cout << "str[" << str << "]" << endl;

	if (doc.Parse<0>(str.c_str()).HasParseError()) {
		throw runtime_error("configuration parse error!");
	}

	// pidfile
	if (!doc.HasMember(cpidfile)) {
		throw runtime_error("pidfile is not configured!");
	}
  if (!doc[cpidfile].IsString()) {
		throw runtime_error("pidfile must be string!");
	}
	pidfile = doc[cpidfile].GetString();

	// appdir
	if (!doc.HasMember(cappdir)) {
		throw runtime_error("appdir is not configured!");
	}
  if (!doc[cappdir].IsString()) {
		throw runtime_error("appdir must be string!");
	}
	appdir = doc[cappdir].GetString();

	// logdir
	if (!doc.HasMember(clogdir)) {
		throw runtime_error("logdir is not configured!");
	}
  if (!doc[clogdir].IsString()) {
		throw runtime_error("logdir must be string!");
	}
	logdir = doc[clogdir].GetString();

	// port
	if (!doc.HasMember(cport)) {
		throw runtime_error("port is not configured!");
	}
  if (!doc[cport].IsInt()) {
		throw runtime_error("port must be integer!");
	}
	port = doc[cport].GetInt();

	// dbHost
	if (!doc.HasMember(cDbHost)) {
		throw runtime_error("db_host is not configured!");
	}
  if (!doc[cDbHost].IsString()) {
		throw runtime_error("db_host must be string!");
	}
	dbHost = doc[cDbHost].GetString();

	// dbPort
	if (!doc.HasMember(cDbPort)) {
		throw runtime_error("db_port is not configured!");
	}
  if (!doc[cDbPort].IsInt()) {
		throw runtime_error("db_port must be integer!");
	}
	dbPort = doc[cDbPort].GetInt();

	// dbName
	if (!doc.HasMember(cDbName)) {
		throw runtime_error("db_name is not configured!");
	}
  if (!doc[cDbName].IsString()) {
		throw runtime_error("db_name must be string!");
	}
	dbName = doc[cDbName].GetString();

	// dbUser
	if (!doc.HasMember(cDbUser)) {
		throw runtime_error("db_user is not configured!");
	}
  if (!doc[cDbUser].IsString()) {
		throw runtime_error("db_user must be string!");
	}
	dbUser = doc[cDbUser].GetString();

	// dbPass
	if (!doc.HasMember(cDbPass)) {
		throw runtime_error("db_pass is not configured!");
	}
  if (!doc[cDbPass].IsString()) {
		throw runtime_error("db_pass must be string!");
	}
	dbPass = doc[cDbPass].GetString();

	// daemon
	daemon = false;
	if (!doc.HasMember(cDaemon)) {
		throw runtime_error("daemon is not configured!");
	}
  if (!doc[cDaemon].IsBool()) {
		throw runtime_error("daemon must be bool!");
	}
	daemon = doc[cDaemon].GetBool();
	if (!doc.HasMember(cDaemon)) {
		throw runtime_error("daemon is not configured!");
	}

	// quote live period
	quoteLivePeriod = 10;
	if (doc.HasMember(cQuoteLivePeriod)) {
		if (!doc[cQuoteLivePeriod].IsNumber()) {
			throw runtime_error("quoteLivePeriod must be number!");
		}
		quoteLivePeriod = doc[cQuoteLivePeriod].GetInt();
		cout << "quoteLivePeriod:" << quoteLivePeriod << endl;
	}

	initialized = true;
	cout << "loaded!" << endl;
}

string Config::getPidFile() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return pidfile;
}

string Config::getAppDir() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return appdir;
}

string Config::getLogDir() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return logdir;
}

int Config::getPort() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return port;
}

string Config::getDbHost() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return dbHost;
}

int Config::getDbPort() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return dbPort;
}

string Config::getDbName() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return dbName;
}

string Config::getDbUser() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return dbUser;
}

string Config::getDbPass() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return dbPass;
}

bool Config::isDaemon() {
	if (!initialized) {
		throw runtime_error("not initialized!");
	}
	return daemon;
}

bool Config::isInitialized() {
	return initialized;
}

AppCtx* Config::getAppCtx() {
	return appCtx;
}

void Config::setAppCtx(AppCtx *ctx) {
	appCtx = ctx;
}

long Config::getQuoteLivePeriod() {
	return quoteLivePeriod;
}

}

