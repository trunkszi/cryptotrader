//
// Created by Quincy on 2022/8/30.
//

#ifndef CRYPTOTRADER_FLAGS_H
#define CRYPTOTRADER_FLAGS_H

#include "co/flag.h"

#if defined(_WIN32)
DEF_string(option, "--service --log_dir C:\\cryptotrader", "set startup parameters");
DEF_bool(service, false, "Run as service mode");
DEF_bool(install, false, "Run as install service");
DEF_bool(uninstall, false, "Run as uninstall service");
DEF_bool(check, false, "Check if the service is installed");
DEF_string(name, "cryptotrader", "Specify service name");
DEF_string(display, "cryptotrader", "Specify the SCM Manager display name");
DEF_string(desc, "", "Specify the content of the service description");
DEF_string(path, "", "Path to the current service executable");
DEF_string(user, "", "Set the user to which the service belongs, the default is the current user");
DEF_string(pass, "", "Set the user to which the service belongs login password, the mode is the current user password");
DEF_string(mode, "auto", "Set the service startup mode, options [auto, manaul], default: auto");
DEF_string(depends, "", "Set service dependency, default: \"\", format: \"service1\\service2\\service3\"");

#endif


#endif //CRYPTOTRADER_FLAGS_H
