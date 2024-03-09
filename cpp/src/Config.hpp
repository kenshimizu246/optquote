
#include <string>
#include <exception>
#include <stdexcept>

#include "AppCtx.hpp"

using namespace std;

#ifndef _Config_hpp
#define _Config_hpp

namespace optquote {

class Config {
	private:
		Config();
		~Config();
		bool initialized;
		string appdir;
		string logdir;
		string pidfile;
		int port;
		string dbHost;
		int dbPort;
		string dbName;
		string dbUser;
		string dbPass;
		bool daemon;
		long quoteLivePeriod;

		AppCtx *appCtx;

	public:
		void load(const char* filename);
		string getAppDir();
		string getLogDir();
		string getPidFile();
		int getPort();
		bool isInitialized();
		string getDbHost();
		int getDbPort();
		string getDbName();
		string getDbUser();
		string getDbPass();

		bool isDaemon();

		long getQuoteLivePeriod();

		AppCtx* getAppCtx();
		void setAppCtx(AppCtx *ctx);

		static Config& getInstance() {
			static Config config;
			return config;
		}
};

}

#endif /*_Config_hpp*/
