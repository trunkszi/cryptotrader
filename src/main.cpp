#include <boost/application.hpp>    // for boost::application
#include <boost/system.hpp>     // for boost::system::error_code
#include <boost/bind.hpp>
#include <string>

#include "co/all.h"     // co::flag co::log

#if defined(_WIN32)

#include <utility>
#include "windows/service_setup.h"

#endif

#include "flags.h"

using namespace boost;

class Application {
public:
    Application(application::global_context_ptr ctx) : _context(std::move(ctx)) {}

    int operator()() {
        auto pid = _context->find<application::process_id>();
        LOG << "[pid]: " << pid->pid();

        auto modes = _context->find<application::run_mode>();
        if (modes->mode() == application::common::mode()) {
            LOG << "Yes am I a common application!";
        }

        if (modes->mode() == application::server::mode()) {
            LOG << "Yes am I a server application!";
        }

        LOG << "your application logic!";

        _context->find<application::wait_for_termination_request>()->wait();
        return 0;
    }

#if defined(BOOST_WINDOWS_API)

    bool pause() {
        LOG << "pause...";
        // true 暂停 false 忽略
        return true;
    }

    bool resume() {
        LOG << "resume...";

        // true 恢复 false 忽略
        return true;
    }

#endif

    bool stop() {
        // 捕获SIGINT、SIGQUIT、SIGKILL信号
        LOG << "stop...";
        // true 停止 false 忽略
        return true;
    }

private:
    application::global_context_ptr _context;
};

#if defined(BOOST_WINDOWS_API)

bool setup(application::global_context_ptr &context, boost::system::error_code &ec) {
    strict_lock<application::aspect_map> guard(*context);


    using namespace boost::application::scm;
    using namespace boost::application;

//    value_type service_user = value_type(""),
//    value_type service_password = value_type(""),
//    value_type service_start_mode = value_type("auto"),
//    value_type service_depends = value_type(""),
//    value_type service_option_string = value_type(""))

    if (FLG_install) {
        InstallService<std::string>(
                std::move(std::string(FLG_name.c_str())),
                std::move(std::string(FLG_display.c_str())),
                std::move(std::string(FLG_desc.c_str())),
                std::move(std::string(FLG_path.c_str())),
                std::move(std::string(FLG_user.c_str())),
                std::move(std::string(FLG_pass.c_str())),
                std::move(std::string(FLG_mode.c_str())),
                std::move(std::string(FLG_depends.c_str())),
                std::move(std::string(FLG_option.c_str()))
        ).install(ec);
    }

    if (FLG_uninstall) {
        UninstallService<std::string>(
                std::move(std::string(FLG_name.c_str())),
                std::move(std::string(FLG_path.c_str()))
        ).uninstall(ec);
    }

    if (FLG_check) {
        return CheckService<std::string>(
                std::move(std::string(FLG_name.c_str()))
        ).exist(ec);
    }
    return true;
}

#endif

int main(int argc, char *argv[]) {
    flag::init(argc, argv);
    log::init();
    application::global_context_ptr app_context = application::global_context::create();
    Application app(app_context);

    // 服务停止回掉注册
    application::handler<>::callback termination_callback = boost::bind(&Application::stop, &app);
    app_context->insert<application::termination_handler>(
            boost::make_shared<application::termination_handler_default_behaviour>(termination_callback));

#if defined(BOOST_WINDOWS_API)
    // windows only : add pause handler
    application::handler<>::callback pause_callback = boost::bind(&Application::pause, &app);
    app_context->insert<application::pause_handler>(
            boost::make_shared<application::pause_handler_default_behaviour>(pause_callback));

    // windows only : add resume handler
    application::handler<>::callback resume_callback = boost::bind(&Application::resume, &app);
    app_context->insert<application::resume_handler>(
            boost::make_shared<application::resume_handler_default_behaviour>(resume_callback));


    boost::system::error_code ec;
    int result = 0;

    // 服务安装
    if (FLG_install) {
        FLG_uninstall = false;
        FLG_check = false;
        setup(app_context, ec);
        goto end;
    } else if (FLG_uninstall) {
        FLG_install = false;
        FLG_check = false;
        FLG_path = FLG_path.size() == 0 ? argv[0] : FLG_path;
        setup(app_context, ec);
        goto end;
    } else if (FLG_check) {
        FLG_install = false;
        FLG_uninstall = false;
        setup(app_context, ec);
        goto end;
    }

    if(!fs::exists("C:\\cryptotrader")){
        fs::mkdir("C:\\cryptotrader");
    }

    // 以服务模式运行
    if (FLG_service) {
        using namespace boost::application::scm;
        using namespace boost::application;

        bool is_exist = CheckService<std::string>(
                std::move(std::string(FLG_name.c_str()))
        ).exist(ec);

        if (is_exist) {

            // 服务已经存在、直接启动服务
            result = application::launch<application::server>(app, app_context, ec);
        } else {
            FLG_install = true;
            // 服务不存在则安装服务并尝试启动服务
            FLG_path = FLG_path.size() == 0 ? std::string(argv[0]) : std::string(FLG_path.c_str());
            if (setup(app_context, ec) && !ec) {
                co::sleep(3000);
                result = application::launch<application::server>(app, app_context, ec);
            }
        }
    } else {
        if(!fs::exists("C:\\cryptotrader")){
            fs::mkdir("C:\\cryptotrader");
        }
        FLG_cout = true;
        // 命令行模式运行
        result = application::launch<application::common>(app, app_context, ec);
    }

#else
    if(!fs::exists("/var/log/cryptotrader")){
        fs::mkdir("/var/log/cryptotrader");
    }

    FLG_cout = true;
    result = application::launch<application::common>(app, app_context, ec);
#endif

    end:
    if (ec) {
        FLOG << "[error: ] " << ec.message() << " " << "[code: ]" << ec.value();
    }
    app_context->destroy();
    return result;
}