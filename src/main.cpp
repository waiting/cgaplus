#include <winux.hpp>
#include <eiennet.hpp>
#include <eientpl.hpp>
#include <eiendb.hpp>

using namespace winux;
using namespace eiennet;
using namespace eientpl;
using namespace eiendb;
using namespace http;
using namespace std;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"
#include "cgaplus_actions.hpp"
#include "cgaplus_pages.hpp"

int main( int argc, const char * argv[] )
{
    SetLocale loc;
    SocketLib init;
    Configure confObj("server.conf");

    HttpServerConfig config(confObj);
    config.outputVerbose = false;
    CgaPlusHttpServer server(config);

    // 页面
    server.setPageHandler( "", Page_dashboard );
    server.setPageHandler( "dashboard", Page_dashboard );
    server.setPageHandler( "quikly", Page_quikly );
    server.setPageHandler( "cgaplussettings", Page_cgaplussettings );
    server.setPageHandler( "accounts", Page_accounts );
    server.setPageHandler( "gids", Page_gids );
    server.setPageHandler( "characters", Page_characters );
    server.setPageHandler( "addchara", Page_addchara );
    server.setPageHandler( "tags", Page_tags );
    server.setPageHandler( "tagview", Page_tagview );

    // 动作
    server.setActionHandler( "startupgame", Action_startupgame );
    server.setActionHandler( "quiklysave", Action_quiklysave );
    server.setActionHandler( "getchara", Action_getchara );
    server.setActionHandler( "changeserver", Action_changeserver );
    server.setActionHandler( "addchara", Action_addchara );
    server.setActionHandler( "checkguiport", Action_checkguiport );
    server.setActionHandler( "checkgameport", Action_checkgameport );
    server.setActionHandler( "cgasetscript", Action_cgasetscript );
    server.setActionHandler( "cgasetsettings", Action_cgasetsettings );
    server.setActionHandler( "cgaplussetsettings", Action_cgaplussetsettings );
    server.setActionHandler( "cgaplusgetsettings", Action_cgaplusgetsettings );
    server.setActionHandler( "detectcgapath", Action_detectcgapath );
    server.setActionHandler( "delaccount", Action_delaccount );
    server.setActionHandler( "delgid", Action_delgid );
    server.setActionHandler( "delchara", Action_delchara );
    server.setActionHandler( "test", Action_test );


    CommandLineVars cmdVars( argc, argv, "", "", "--no-open-browser" );

    if ( !cmdVars.hasFlag("--no-open-browser") )
    {
        auto hosts = ip::Resolver("", 9456).getArr();
        auto hostStr = hosts[hosts.size() - 1].toString();
        // 启动浏览器
        String mainPageUrl = Format( "http://127.0.0.1:%u/", config.serverPort );
        cout << "open " << mainPageUrl << endl;
        ShellExecute( NULL, "open", mainPageUrl.c_str(), NULL, NULL, SW_NORMAL );
    }

    // 开启http服务
    server.run();
    return 0;
}
